#include "duckdb_llama_cpp.hpp"

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

#define USE_CACHE 1

namespace duckdb {
LlamaCppPredictor::LlamaCppPredictor(std::string prompt)
    : Predictor(), prompt(std::move(prompt)) {
}

/**
 * Configure the predictor. Reads the options set in `CREATE MODEL`
 * via `options` parameter or use the defaults set in the client configs.
 *
 * Called only once per query before LlamaCppPredictor::Load() is being called.
 *
 * @param client_config contains default or manually set configs.
 * @param options map of overrides for the client configs set when model is uploaded.
 */
void LlamaCppPredictor::Config(const ClientConfig &client_config, const case_insensitive_map_t<Value> &options) {
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

	// Disable llama logging
	llama_log_set(
	    [](ggml_log_level /*level*/, const char * /*text*/, void * /*user_data*/) {
		    // noop
	    },
	    NULL);

	// TODO: Implement any configurations here.
	this->n_gl = 99;
	this->n_predict = 256;
}

/**
 * Loads the models required by the predictor.
 *
 * Called only once per query before LlamaCppPredictor::PredictChunk() calls.
 *
 * @param model_path path of the model set in `CREATE MODEL`.
 * @param stats statistics for profiling. Method should update `load` with the time this method use.
 */
void LlamaCppPredictor::Load(const std::string &model_path, unique_ptr<PredictStats> &stats) {
#if OPT_TIMING
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif
	D_ASSERT(this->task == PredictorTask::PREDICT_LLM_TASK);

	// This would be the specific model name or API URL.
	this->model_path = model_path;
	std::cout << "Model Path: " << model_path << std::endl;

	// init model
	llama_model_params model_params = llama_model_default_params();
	model_params.vocab_only = false;

	this->model = llama_model_load_from_file(model_path.c_str(), model_params);
	if (this->model == NULL) {
		throw InternalException("Unable to load model: " + model_path);
	}

	// init vocabulary
	this->vocab = llama_model_get_vocab(model);

	GenerateGrammar();
	InitializeSampler();

#if OPT_TIMING
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	stats->load = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
#endif
}

void LlamaCppPredictor::GenerateGrammar() {
	std::vector<std::pair<std::string, LogicalTypeId>> attrs {};
	prompt_util.process_prompt_and_extract_types(attrs, this->prompt);

	std::string rule_base_prefix = R"(ws "\")";
	std::string rule_base_str = R"(\":" ws string )";
	std::string rule_base_integer = R"(\":" ws integer )";
	std::string rule_base_double = R"(\":" ws double )";
	std::string rule_base_bool = R"(\":" ws boolean )";

	std::stringstream ss;
	bool first = true;
	for (auto attr : attrs) {
		if (!first) {
			ss << R"("," )";
		} else {
			first = false;
		}

		ss << R"( ws "\")" << attr.first;
		switch (attr.second) {
		case LogicalTypeId::VARCHAR:
			ss << rule_base_str;
			break;
		case LogicalTypeId::INTEGER:
			ss << rule_base_integer;
			break;
		case LogicalTypeId::DOUBLE:
			ss << rule_base_double;
			break;
		case LogicalTypeId::BOOLEAN:
			ss << rule_base_bool;
			break;
		default:
			throw InternalException("Unsupported result type");
		}
	}
	std::string replacement = ss.str();

	std::string grammar_prefix = R"(root ::= DuckMLGE
DuckMLGE ::= "{"   )";
	if (is_source) {
		grammar_prefix = R"(root ::= DuckMLGElist
DuckMLGE ::= "{"   )";
	}

	std::string grammar_suffix = R"(   "}"
DuckMLGElist ::= "[]" | "["   ws   DuckMLGE   (","   ws   DuckMLGE)*   "]"
string ::= "\""   ([^"]*)   "\""
boolean ::= "true" | "false"
ws ::= [ ]*
integer ::= "-"? [0-9]+
double  ::= "-"? [0-9]+ "." [0-9]+
stringlist ::= "["   ws   "]" | "["   ws   string   (","   ws   string)*   ws   "]"
)";

	this->grammar = grammar_prefix + replacement + grammar_suffix;

	std::cout << "Prompt: " << this->prompt << std::endl;
	std::cout << "Grammer:" << this->grammar << std::endl;
}

void LlamaCppPredictor::InitializeSampler() {
	// init the sampler
	auto lparams = llama_sampler_chain_default_params();
	lparams.no_perf = false;

	this->grmr = llama_sampler_init_grammar(this->vocab, this->grammar.c_str(), "root");
	this->chain = llama_sampler_chain_init(lparams);

	std::vector<llama_logit_bias> logit_bias; // logit biases to apply
	llama_sampler_chain_add(this->chain, llama_sampler_init_logit_bias(llama_vocab_n_tokens(this->vocab),
	                                                                   logit_bias.size(), logit_bias.data()));

	int32_t penalty_last_n = 64;
	float penalty_repeat = 1.00f;
	float penalty_freq = 0.00f;
	float penalty_present = 0.00f;
	llama_sampler_chain_add(
	    this->chain, llama_sampler_init_penalties(penalty_last_n, penalty_repeat, penalty_freq, penalty_present));

	float dry_multiplier = 0.0f; // 0.0 = disabled;      DRY repetition penalty for tokens extending repetition:
	float dry_base =
	    1.75f; // 0.0 = disabled;      multiplier * base ^ (length of sequence before token - allowed length)
	int32_t dry_allowed_length = 2;  // tokens extending repetitions beyond this receive penalty
	int32_t dry_penalty_last_n = -1; // how many tokens to scan for repetitions (0 = disable penalty, -1 = context size)
	std::vector<std::string> dry_sequence_breakers = {"\n", ":", "\"", "*"}; // default sequence breakers for DRY
	std::vector<const char *> c_breakers;
	c_breakers.reserve(dry_sequence_breakers.size());
	for (const auto &str : dry_sequence_breakers) {
		c_breakers.push_back(str.c_str());
	}
	llama_sampler_chain_add(this->chain,
	                        llama_sampler_init_dry(vocab, llama_model_n_ctx_train(this->model), dry_multiplier,
	                                               dry_base, dry_allowed_length, dry_penalty_last_n, c_breakers.data(),
	                                               c_breakers.size()));

	int32_t top_k = 40;
	llama_sampler_chain_add(this->chain, llama_sampler_init_top_k(top_k));

	int32_t min_keep = 0; // 0 = disabled, otherwise samplers should return at least min_keep tokens
	float typ_p = 1.00f;  // typical_p, 1.0 = disabled
	llama_sampler_chain_add(this->chain, llama_sampler_init_typical(typ_p, min_keep));

	float top_p = 0.95f; // 1.0 = disabled
	llama_sampler_chain_add(this->chain, llama_sampler_init_top_p(top_p, min_keep));

	float min_p = 0.05f; // 0.0 = disabled
	llama_sampler_chain_add(this->chain, llama_sampler_init_min_p(min_p, min_keep));

	float xtc_probability = 0.00f; // 0.0 = disabled
	float xtc_threshold = 0.10f;   // > 0.5 disables XTC
	llama_sampler_chain_add(this->chain,
	                        llama_sampler_init_xtc(xtc_probability, xtc_threshold, min_keep, LLAMA_DEFAULT_SEED));

	float temp = 0.80f;              // <= 0.0 to sample greedily, 0.0 to not output probabilities
	float dynatemp_range = 0.00f;    // 0.0 = disabled
	float dynatemp_exponent = 1.00f; // controls how entropy maps to temperature in dynamic temperature sampler
	llama_sampler_chain_add(this->chain, llama_sampler_init_temp_ext(temp, dynatemp_range, dynatemp_exponent));

	llama_sampler_chain_add(this->chain, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));
}

std::string generate(llama_model *model, const llama_vocab *vocab, llama_sampler *grmr, llama_sampler *chain,
                     const std::string &prompt, const int n_predict) {
	std::string response;
	const int n_prompt = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), NULL, 0, true, true);

	// allocate space for the tokens and tokenize the prompt
	std::vector<llama_token> prompt_tokens(n_prompt);
	if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), true, true) <
	    0) {
		throw InternalException("Unable to tokenize the prompt: " + prompt);
	}

	// initialize the context
	llama_context_params ctx_params = llama_context_default_params();
	ctx_params.n_seq_max = 1;
	ctx_params.n_ctx = 4096;
	ctx_params.n_batch = 2048;
	ctx_params.n_ubatch = 512;
	ctx_params.no_perf = true;

	std::cout << "Prompt: " << prompt << std::endl;
	std::cout << "Num. Prompt Tokens: " << n_prompt << std::endl;
	std::cout << "Num. Predict Tokens: " << n_predict << std::endl;

	int n_threads = 32;
	llama_context *ctx = llama_init_from_model(model, ctx_params);
	llama_set_n_threads(ctx, n_threads, n_threads);

	// prepare a batch for the prompt
	llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());
	int n_decode = 0;
	llama_token new_token_id;
	for (int n_pos = 0; n_pos + batch.n_tokens < n_prompt + n_predict;) {
		if (llama_decode(ctx, batch)) {
			throw InternalException("Failed to llama_decode");
		}

		n_pos += batch.n_tokens;

		// sample the next token
		std::vector<llama_token_data> cur;
		llama_token_data_array cur_p;

		const auto *logits = llama_get_logits_ith(ctx, -1);
		const int n_vocab = llama_vocab_n_tokens(vocab);
		cur.resize(n_vocab);

		for (llama_token token_id = 0; token_id < n_vocab; token_id++) {
			cur[token_id] = llama_token_data {token_id, logits[token_id], 0.0f};
		}
		cur_p = {cur.data(), cur.size(), -1, false};

		llama_sampler_apply(chain, &cur_p);

		if (cur_p.selected == -1) {
			throw InternalException("no selected token during sampling - check your sampling configuration");
		}
		const llama_token id = cur_p.data[cur_p.selected].id;

		// check if it the sampled token fits the grammar
		bool found = false;
		{
			llama_token_data single_token_data = {id, 1.0f, 0.0f};
			llama_token_data_array single_token_data_array = {&single_token_data, 1, -1, false};

			llama_sampler_apply(grmr, &single_token_data_array);

			const bool is_valid = single_token_data_array.data[0].logit != -INFINITY;
			if (is_valid) {
				new_token_id = id;
				found = true;
			}
		}

		if (!found) {
			// resampling:
			// if the token is not valid, sample again, but first apply the grammar sampler and then the sampling chain
			for (llama_token token_id = 0; token_id < n_vocab; token_id++) {
				cur[token_id] = llama_token_data {token_id, logits[token_id], 0.0f};
			}
			cur_p = {cur.data(), cur.size(), -1, false};

			llama_sampler_apply(grmr, &cur_p);
			llama_sampler_apply(chain, &cur_p);

			new_token_id = cur_p.data[cur_p.selected].id;
		}

		llama_sampler_accept(grmr, new_token_id);
		llama_sampler_accept(chain, new_token_id);

		// is it an end of generation?
		if (llama_vocab_is_eog(vocab, new_token_id)) {
			break;
		}

		// convert the token to a string, print it and add it to the response
		char buf[256];
		int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
		if (n < 0) {
			throw InternalException("Failed to convert token to piece");
		}
		std::string piece(buf, n);
		response += piece;

		// prepare the next batch with the sampled token
		batch = llama_batch_get_one(&new_token_id, 1);

		n_decode += 1;
	}
	llama_free(ctx);

	return response;
};

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
void LlamaCppPredictor::PredictChunk(ClientContext &client, DataChunk &input, DataChunk &output, int rows,
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

		for (int i = frow; i < lrow; ++i) {
			std::string llm_out {};
			std::string embeded = prompt_util.embed_prompt(this->prompt, i, input, info);
#ifdef USE_CACHE
			if (cache.find(embeded) != cache.end()) {
				std::cout << "Cache hit!" << std::endl;
				llm_out = cache[embeded];
			} else {
#endif
				std::string rewritten = this->prompt + ";\n" + embeded;
				// std::string rewritten = StringUtil::Format("<s>[INST] %s input=\"%s\" [/INST]", this->prompt, input_str);

				llm_out = generate(this->model, this->vocab, this->grmr, this->chain, rewritten, this->n_predict);

				llama_sampler_reset(this->grmr);
				llama_sampler_reset(this->chain);
#ifdef USE_CACHE
				cache[embeded] = llm_out;
			}
#endif
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

void LlamaCppPredictor::ScanChunk(ClientContext &client, DataChunk &output, const PredictInfo &info, unique_ptr<PredictStats> &stats) {
#if OPT_TIMING
	stats->move = 0;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

	std::string rewritten = this->prompt + ". Always produce a list.";

	std::string llm_out {};
	llm_out = generate(this->model, this->vocab, this->grmr, this->chain, rewritten, this->n_predict);

	llama_sampler_reset(this->grmr);
	llama_sampler_reset(this->chain);
	
	std::cout << llm_out << "||" << std::endl;
	prompt_util.extract_array_data(llm_out, output, 0, info);

#if OPT_TIMING
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	stats->predict += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

	stats->move_rev = 0;
#endif
}

} // namespace duckdb
 