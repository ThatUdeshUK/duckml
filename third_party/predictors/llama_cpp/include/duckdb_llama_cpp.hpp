#pragma once

#include <string>
#include <map>
#include "duckdb/common/types/vector.hpp"
#include "duckdb/execution/operator/projection/physical_predict.hpp"
#include "../../common/common.hpp"

#include "llama.h"

#define OPT_TIMING 1

namespace duckdb {
class LlamaCppPredictor final : public Predictor {
public:
	int n_gl;
	int n_predict;
	std::string prompt;
	std::string base_api;
	std::string grammar;
	bool is_api;
	bool is_source = false;

	llama_model *model;

	const llama_vocab *vocab;

	llama_sampler *grmr;
	llama_sampler *chain;

private:
	PromptUtil prompt_util;

public:
	LlamaCppPredictor(std::string prompt);
	~LlamaCppPredictor() override {
		if (!is_api) {
			llama_sampler_free(grmr);
			llama_sampler_free(chain);
			llama_model_free(model);
		}
	}

public:
	void Config(const ClientContext &context, const case_insensitive_map_t<Value> &options) override;
	void Load(ClientContext &context, const std::string &model_path, unique_ptr<PredictStats> &stats) override;
	void PredictChunk(ClientContext &client, DataChunk &input, DataChunk &output, idx_t rows, const PredictInfo &info,
	                  unique_ptr<PredictStats> &stats) override;
	void ScanChunk(ClientContext &client, DataChunk &output, const PredictInfo &info,
	               unique_ptr<PredictStats> &stats) override;

private:
	void GenerateGrammar();
	void InitializeSampler();
};
} // namespace duckdb
