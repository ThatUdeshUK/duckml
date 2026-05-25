#pragma once

#include <string>
#include <memory>

#include "duckdb/common/types/vector.hpp"
#include "duckdb/execution/operator/projection/physical_predict.hpp"
#include "../../common/common.hpp"

#include "openai.hpp"

#define OPT_TIMING 1

namespace duckdb {

struct BatchResult {
	size_t tokens;
	size_t in_tokens;
	size_t out_tokens;
	int64_t time;
	idx_t frow;
	idx_t n_rows;
	idx_t n_calls;
	bool is_concat;
	std::vector<std::string> outputs;
	std::vector<std::vector<float>> embeddings;

	bool Success() const {
		return n_calls == outputs.size() || n_rows == embeddings.size();
	}
};

class LlmApiPredictor : public Predictor {
public:
	int n_predict;
	std::string prompt;
	std::string base_api;
	std::string secret;
	std::string grammar;

	idx_t n_threads;
	idx_t req_per_min;

private:
	PromptUtil prompt_util;
	unique_ptr<OpenAI> api;

public:
	LlmApiPredictor(std::string prompt, std::string base_api, std::string secret);

public:
	void Config(const ClientContext &context, const case_insensitive_map_t<Value> &options) override;
	void Load(ClientContext &client, const std::string &path, unique_ptr<PredictStats> &stats) override;
	void PredictChunk(ClientContext &client, DataChunk &input, DataChunk &output, const idx_t rows,
	                  const PredictInfo &info, unique_ptr<PredictStats> &stats) override;
	vector<string> PredictString(ClientContext &client, vector<string> &input, const PredictInfo &info) override;
	void ScanChunk(ClientContext &client, DataChunk &output, const PredictInfo &info,
	               unique_ptr<PredictStats> &stats) override;
	std::unique_ptr<BatchResult> PredictBatch(OpenAI &api, const vector<string> &input, const idx_t rows, idx_t batch,
	                                          idx_t batch_size);
	std::unique_ptr<BatchResult> PredictEmbedBatch(OpenAI &api, const vector<string> &input, const idx_t rows,
	                                               idx_t batch, idx_t batch_size);
	std::unique_ptr<BatchResult> PredictOne(OpenAI &api, const string &input, idx_t row);
	std::unique_ptr<BatchResult> PredictAgg(OpenAI &api, const string &input);
	vector<string> ApplyOrderStrat(vector<string> &rows, vector<idx_t> &orig_order) const;
	void PredictJoin(ClientContext &client, DataChunk &input, DataChunk &output, const idx_t rows,
	                 const idx_t n_left_cols, const PredictInfo &info, unique_ptr<PredictStats> &stats) override;

private:
	void GenerateGrammar();
	std::string GenerateSystemMessage(bool is_array) const;
};
} // namespace duckdb
