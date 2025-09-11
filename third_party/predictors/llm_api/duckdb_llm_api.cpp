#include "duckdb_llm_api.hpp"

#include "duckdb/main/extension_helper.hpp"
#include "PerfEvent.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <string>
#include <utility>

namespace duckdb {
LlmApiPredictor::LlmApiPredictor(std::string prompt, std::string base_api, std::string secret)
    : Predictor(), prompt(std::move(prompt)), base_api(std::move(base_api)), secret(std::move(secret)) {
}

/**
 * Configure the predictor. Reads the options set in `CREATE MODEL`
 * via `options` parameter or use the defaults set in the client configs.
 *
 * Called only once per query before LlmApiPredictor::Load() is being called.
 *
 * @param client_config contains default or manually set configs.
 * @param options map of overrides for the client configs set when model is uploaded.
 */
void LlmApiPredictor::Config(const ClientConfig &client_config, const case_insensitive_map_t<Value> &options) {
	this->batch_size = (options.find("batch_size") != options.end()) ? IntegerValue::Get(options.at("batch_size"))
	                                                                 : client_config.ml_batch_size;
	this->llm_max_tokens = (options.find("llm_max_tokens") != options.end())
	                           ? IntegerValue::Get(options.at("llm_max_tokens"))
	                           : client_config.llm_max_tokens;
	this->use_cache = (options.find("use_cache") != options.end()) ? BooleanValue::Get(options.at("use_cache"))
	                                                                 : client_config.llm_use_cache;
	this->use_batch = (options.find("use_batch") != options.end())
	                           ? BooleanValue::Get(options.at("use_batch"))
	                           : client_config.llm_use_batch;
	
	// TODO: Implement any configurations here.
	this->n_predict = 64;
}

/**
 * Loads the models required by the predictor.
 *
 * Called only once per query before LlmApiPredictor::PredictChunk() calls.
 *
 * @param model_path path of the model set in `CREATE MODEL`.
 * @param stats statistics for profiling. Method should update `load` with the time this method use.
 */
void LlmApiPredictor::Load(const std::string &model_path, unique_ptr<PredictStats> &stats) {
#if OPT_TIMING
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif
	D_ASSERT(this->task == PredictorTask::PREDICT_LLM_TASK);

	// This would be the specific model name or API URL.
	this->model_path = model_path;
	std::cout << "Model Path: " << model_path << std::endl;
	std::cout << "Base API: " << this->base_api << std::endl;

	GenerateGrammar();

#if OPT_TIMING
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	stats->load = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
#endif
}

void LlmApiPredictor::GenerateGrammar() {
	std::vector<std::pair<std::string, LogicalTypeId>> attrs {};
	
	prompt_util.process_prompt_and_extract_types(attrs, this->prompt);

	std::stringstream ss;
	ss << "{";
	bool first = true;
	for (auto attr : attrs) {
		if (!first) {
			ss << R"(, )";
		} else {
			first = false;
		}

		ss << R"(")" << attr.first << R"(" : )";
		switch (attr.second) {
		case LogicalTypeId::VARCHAR:
			ss << R"("<string>")";
			break;
		case LogicalTypeId::INTEGER:
			ss << R"("<integer>")";
			break;
		case LogicalTypeId::DOUBLE:
			ss << R"("<double>")";
			break;
		case LogicalTypeId::BOOLEAN:
			ss << R"("<boolean>")";
			break;
		default:
			throw InternalException("Unsupported result type");
		}
	}
	ss << "}";
	std::string replacement = ss.str();

	this->grammar = replacement;

	std::cout << "Prompt: " << this->prompt << std::endl;
	std::cout << "Grammer:" << this->grammar << std::endl;
}

std::string LlmApiPredictor::GenerateSystemMessage(bool is_array) const {
	std::string suffix = R"(. Do not include any extra text, explanations, language specifier, produce {<key>: <single value>} for JSON objects. The JSON must be parsable by a standard parser.)";
	if (is_array) {
		return R"(You are a helpful assistant. Always respond **only** with valid JSON array where each object is in format )" + this->grammar + suffix;
	}
	return R"(You are a helpful assistant. Always respond **only** with valid JSON object (i.e. not an array) in format )" + this->grammar + suffix;
}

/**
 * Infer the models for a chunk (column vectors containing tupes).
 *
 * Called multiple times until all the tuples from child operator is covered.
 *
 * @param input chunk containing the output of child operator.
 * @param output chunk we should produce (predicted columns vectors).
 * @param rows number of rows in the input chunk (= output chunk).
 * @param info BoundPredictInfo struct with model and operator metadata
 * @param stats statistics for profiling. Method should update `predict` with the time this method use.
 */
void LlmApiPredictor::PredictChunk(ClientContext &client, DataChunk &input, DataChunk &output, int rows,
                                     const PredictInfo &info, unique_ptr<PredictStats> &stats) {
	// Implemented to support mini-batches (i.e., batch_size < vector_size).
	// However, unless changed, batch_size == vector_size by default.
	// Check `ml_batch_size` in ClientConfig at `src/include/duckdb/main/client_config.hpp`
	int rounds = rows / batch_size;
	if (rows % batch_size != 0)
		rounds++;

	for (size_t batch = 0; batch < rounds; batch++) {
#if OPT_TIMING
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

		int frow = batch * batch_size;                // Offset of first row
		int lrow = std::min(frow + batch_size, rows); // Offset of last row
		int num_rows = lrow - frow;                   // Number of rows in the batch

		int cols = info.input_mask.size();

#if OPT_TIMING
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		stats->move += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

		begin = std::chrono::steady_clock::now();
#endif 

		auto &db = DatabaseInstance::GetDatabase(client);
		auto &api = openai::start(db, base_api, secret);

		if (use_batch) {
			std::string llm_out {};

			std::stringstream ss;
			ss << this->prompt + "; \nRespond with a JSON object for each the following " << num_rows << " inputs:\n[";
			for (int i = frow; i < lrow; ++i) {
				ss << "{" << prompt_util.embed_prompt(this->prompt, i, input, info, true) << "},\n";
			}
			ss << "]";

			std::string rewritten = ss.str();
			std::cout << rewritten << std::endl;

			nlohmann::json request;
			request["model"] = this->model_path;
			request["messages"] = {
				{{"content", GenerateSystemMessage(true)}, {"role", "system"}},
				{{"content", rewritten}, {"role", "user"}}};
			// request["max_tokens"] = 64;
			// request["temperature"] = 0;

			auto completion = api.post("chat/completions", request);
			for (auto &msg : completion["choices"]) {
				llm_out = msg["message"]["content"].get<std::string>();
			}
			std::cout << llm_out << "||" << std::endl;
			prompt_util.extract_array_data(llm_out, output, frow, info);
			continue;
		}

		for (int i = frow; i < lrow; ++i) {
			std::string llm_out {};

			std::string embeded = prompt_util.embed_prompt(this->prompt, i, input, info);
			if (use_cache && cache.find(embeded) != cache.end()) {
				std::cout << "Cache hit!" << std::endl;
				llm_out = cache[embeded];
			} else {
				std::string rewritten = this->prompt + ";\n" + embeded;

				nlohmann::json request;
				request["model"] = this->model_path;
				request["messages"] = {
					{{"content", GenerateSystemMessage(false)}, {"role", "system"}},
					{{"content", rewritten}, {"role", "user"}}};
				// request["max_tokens"] = 64;
				// request["temperature"] = 0;

				// auto req_ts = std::chrono::steady_clock::now();
				auto completion = api.post("chat/completions", request);
				// auto req_te = std::chrono::steady_clock::now();
				// auto req_time = std::chrono::duration_cast<std::chrono::seconds>(req_te - req_ts).count();
				// std::cout << "Request time (s):" << req_time << std::endl;

				for (auto &msg : completion["choices"]) {
					llm_out = msg["message"]["content"].get<std::string>();
				}

				if (use_cache)
					cache[embeded] = llm_out;
			}
			std::cout << llm_out << "||" << std::endl;
			prompt_util.extract_row_data(llm_out, i, output, info);
		}

#if OPT_TIMING
		end = std::chrono::steady_clock::now();
		stats->predict += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

		begin = std::chrono::steady_clock::now();
#endif

#if OPT_TIMING
		end = std::chrono::steady_clock::now();
		stats->move_rev += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
#endif

		// TODO: Calculate the accuracy metric here for the prediction and update the stats.correct and stats.total
		// stats->correct += <no_of_positives>;
		// stats->total += rows;
	}
}

void LlmApiPredictor::ScanChunk(ClientContext &client, DataChunk &output, const PredictInfo &info, unique_ptr<PredictStats> &stats) {
#if OPT_TIMING
	stats->move = 0;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

	auto &db = DatabaseInstance::GetDatabase(client);
	auto &api = openai::start(db, base_api, secret);

	std::string rewritten =this->prompt;

	std::string llm_out {};
	nlohmann::json request;

	request["model"] = this->model_path;
	request["messages"] = {
		{{"content", GenerateSystemMessage(true)}, {"role", "system"}},
		{{"content", rewritten}, {"role", "user"}}
	};
	// request["max_tokens"] = 64;
	// request["temperature"] = 0;

	// std::cout << request.dump(2) << std::endl;

	auto completion = api.post("chat/completions", request);
	for (auto &msg : completion["choices"]) {
		llm_out = msg["message"]["content"].get<std::string>();
	}
	// llm_out = llm_out.substr(1, llm_out.size() - 2);
	
	std::cout << llm_out << "||" << std::endl;
	prompt_util.extract_array_data(llm_out, output, 0, info);

#if OPT_TIMING
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	stats->predict += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

	stats->move_rev = 0;
#endif
}

} // namespace duckdb
 