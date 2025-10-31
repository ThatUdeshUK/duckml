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
#include <future>
#include <thread>

#define LLM_USE_THREADS 1

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
	this->n_threads = (options.find("n_threads") != options.end())
							? IntegerValue::Get(options.at("n_threads"))
							: client_config.llm_no_threads;
	this->req_per_min = (options.find("req_per_min") != options.end())
							? IntegerValue::Get(options.at("req_per_min"))
							: 500;
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

std::unique_ptr<BatchResult> LlmApiPredictor::PredictBatch(ClientContext &client, openai::OpenAI& api, DataChunk &input, int rows, int batch, int batch_size, const PredictInfo &info) {
	auto result = make_uniq<BatchResult>();
	
	int tokens = 0;

	int frow = batch * batch_size;                // Offset of first row
	int lrow = std::min(frow + batch_size, rows); // Offset of last row
	int num_rows = lrow - frow;                   // Number of rows in the batch

	int cols = info.input_mask.size();

	if (use_batch) {
		std::string llm_out {};

		std::stringstream ss;
		ss << this->prompt + "; \nRespond with a JSON object for each the following " << num_rows << " inputs:\n[";
		for (int i = frow; i < lrow; ++i) {
			ss << "{" << prompt_util.embed_prompt(this->prompt, i, input, info, true) << "},\n";
		}
		ss << "]";

		std::string rewritten = ss.str();
		std::cout << "Prompt len: " << rewritten.size() << std::endl;

		nlohmann::json request;
		request["model"] = this->model_path;
		request["messages"] = {
			{{"content", GenerateSystemMessage(true)}, {"role", "system"}},
			{{"content", rewritten}, {"role", "user"}}};
		// request["max_tokens"] = 64;
		// request["temperature"] = 0;

		auto req_ts = std::chrono::steady_clock::now();
		auto completion = api.post("chat/completions", request);
		auto req_te = std::chrono::steady_clock::now();
		auto req_time = std::chrono::duration_cast<std::chrono::seconds>(req_te - req_ts).count();
		std::cout << "Batch request time (s):" << req_time << std::endl;
		if (completion.contains("error")) {
			std::cout << "Batch call failed! Falling back to rowise calls. Error: " << completion["error"] << std::endl;
			if (completion["code"] == 429) {
				std::cout << "Wait because too much requests!" << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(30));
			}
		} else {
			tokens += completion["usage"]["total_tokens"].get<int>();

			for (auto &msg : completion["choices"]) {
				llm_out = msg["message"]["content"].get<std::string>();
			}
			std::cout << llm_out << "||" << std::endl;

			result->outputs.push_back(llm_out);
			result->tokens = tokens;
			result->predict = req_time;
			result->is_concat = true;
			result->n_calls = 1;
			return std::move(result);
		}
	}

	long total_time = 0;
	for (int i = frow; i < lrow; ++i) {
		std::string llm_out {};

		std::string embeded = prompt_util.embed_prompt(this->prompt, i, input, info);
		if (use_cache && this->cache.find(embeded) != this->cache.end()) {
			std::cout << "Cache hit!" << std::endl;
			llm_out = this->cache[embeded];
		} else {
			std::string rewritten = this->prompt + ";\n" + embeded;

			nlohmann::json request;
			request["model"] = this->model_path;
			request["messages"] = {
				{{"content", GenerateSystemMessage(false)}, {"role", "system"}},
				{{"content", rewritten}, {"role", "user"}}};
			// request["max_tokens"] = 64;
			// request["temperature"] = 0;

			auto req_ts = std::chrono::steady_clock::now();
			auto completion = api.post("chat/completions", request);
			auto req_te = std::chrono::steady_clock::now();
			auto req_time = std::chrono::duration_cast<std::chrono::seconds>(req_te - req_ts).count();
			std::cout << "Request time (s):" << req_time << std::endl;
			if (completion.contains("error")) {
				std::cout << "LLM call failed! Error: " << completion["error"] << std::endl;
				if (completion["code"] == 429) {
					std::cout << "Wait because too much requests!" << std::endl;
					std::this_thread::sleep_for(std::chrono::seconds(30));
				}
				result->outputs.push_back("");
				continue;
			}
			total_time += req_time;
			tokens += completion["usage"]["total_tokens"].get<int>();

			for (auto &msg : completion["choices"]) {
				llm_out = msg["message"]["content"].get<std::string>();
			}

			std::string cache_out = llm_out;
			if (use_cache)
				this->cache[embeded] = std::move(cache_out);
		}
		std::cout << llm_out << "||" << std::endl;
		result->outputs.push_back(llm_out);
	}
	result->tokens = tokens;
	result->predict = total_time;
	result->is_concat = false;
	result->n_calls = num_rows;
	return std::move(result);
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
    std::cout << "No. tuples: " << rows << std::endl;
#if LLM_USE_THREADS
    std::cout << "No. of threads: " << this->n_threads << std::endl;
    std::cout << "Requests per min: " << this->req_per_min << std::endl;
#endif

	int rounds = rows / batch_size;
	if (rows % batch_size != 0)
		rounds++;
	
	if (use_batch)
		std::cout << "Batch Size: " << batch_size << ", Rounds: " << rounds << std::endl;

#if OPT_TIMING
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

	auto &db = DatabaseInstance::GetDatabase(client);
	auto &api = openai::start(db, base_api, secret);

#if LLM_USE_THREADS
    std::vector<std::future<std::unique_ptr<BatchResult>>> futures;
	size_t total_tokens = 0;
	size_t sub_reqs = 0;
	size_t sub_secs = 0;
	for (size_t batch = 0; batch < rounds; batch = batch + n_threads) {
		std::chrono::steady_clock::time_point b_ts = std::chrono::steady_clock::now();
		for (size_t run = 0; run < n_threads && (batch + run) * batch_size < rows; run++) {
			int batch_i = batch + run;
			futures.push_back(std::async(std::launch::async, &LlmApiPredictor::PredictBatch, this, std::ref(client), std::ref(api), std::ref(input), rows, batch_i, batch_size, std::cref(info)));
		}
		size_t run = 0;
		for (auto &f : futures) {
			int frow = (batch + run) * batch_size;
			auto result = f.get();
			sub_reqs += result->n_calls;
			total_tokens += result->tokens;
			if (result->is_concat) {
				prompt_util.extract_array_data(result->outputs[0], output, frow, info);
			} else {
				auto n_rows = result->outputs.size();
				for (size_t i = 0; i < n_rows; i++) {
					prompt_util.extract_row_data(result->outputs[i], frow + i, output, info);
				}
			}
			run++;
		}
		futures.clear();
		std::chrono::steady_clock::time_point b_te = std::chrono::steady_clock::now();
		sub_secs += std::chrono::duration_cast<std::chrono::seconds>(b_te - b_ts).count();
		
		// if (sub_reqs - n_threads > req_per_min) {
		// 	// We might over that request per min limit
		// 	if (sub_secs < 60) {
		// 		std::this_thread::sleep_for(std::chrono::seconds(60 - sub_secs));
		// 	}
		// 	sub_reqs = 0;
		// 	sub_secs = 0;
		// }
	}
#else
	for (size_t batch = 0; batch < rounds; batch++) {
		int frow = batch * batch_size;                // Offset of first row
		auto result = PredictBatch(client, api, input, rows, batch, batch_size, info);
		total_tokens += result->tokens;
		if (result->is_concat) {
			prompt_util.extract_array_data(result->outputs[0], output, frow, info);
		} else {
			auto n_rows = result->outputs.size();
			for (size_t i = 0; i < n_rows; i++) {
				prompt_util.extract_row_data(result->outputs[i], frow + i, output, info);
			}
		}
	}
#endif
	std::cout << "Total tokens: " << total_tokens << std::endl;

#if OPT_TIMING
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	long total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	stats->predict += total_time;
#endif
	std::cout << "Total time (s): " << total_time * 1.0 / 1000000 << std::endl;
}

string LlmApiPredictor::PredictString(ClientContext &client, string &input, const PredictInfo &info) {
#if OPT_TIMING
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

	auto &db = DatabaseInstance::GetDatabase(client);
	auto &api = openai::start(db, base_api, secret);

	std::string rewritten = this->prompt + "; Consider all of the following inputs: \n" + input;
	std::cout << "prompt: \n" << rewritten << std::endl; 

	std::string llm_out {};
	nlohmann::json request;

	request["model"] = this->model_path;
	auto sys_msg = R"(You are a helpful assistant. Always respond with a plain text. Do not include any explanations or language specifiers.)";
	request["messages"] = {
		{{"content", sys_msg}, {"role", "system"}},
		{{"content", rewritten}, {"role", "user"}}
	};

	auto completion = api.post("chat/completions", request);
	for (auto &msg : completion["choices"]) {
		llm_out = msg["message"]["content"].get<std::string>();
	}

	int tokens = completion["usage"]["total_tokens"].get<int>();
	// std::cout << llm_out << "||" << std::endl;

	std::cout << "Total tokens: " << tokens << std::endl;

#if OPT_TIMING
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	long total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	std::cout << "Call time (s): " << total_time * 1.0 / 1000000 << std::endl;
#endif
	return llm_out;
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

	int tokens = completion["usage"]["total_tokens"].get<int>();
	std::cout << llm_out << "||" << std::endl;

	std::cout << "Total tokens: " << tokens << std::endl;

	prompt_util.extract_array_data(llm_out, output, 0, info, true);

#if OPT_TIMING
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	long total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	stats->predict += total_time;
	std::cout << "Total time (s): " << total_time * 1.0 / 1000000 << std::endl;

	stats->move_rev = 0;
#endif
}

} // namespace duckdb
 