#pragma once

#include <string>
#include <memory>
#include <map>
#include "duckdb/common/types/vector.hpp"
#include "duckdb/execution/operator/projection/physical_predict.hpp"
#include "../../common/common.hpp"

#include "openai.hpp"

#define OPT_TIMING 1

namespace duckdb {

struct BatchResult {
    size_t tokens;
    int n_calls;
	int64_t move;
	int64_t predict;
	int64_t move_rev;
    bool is_concat;
    std::vector<std::string> outputs;
};

class LlmApiPredictor: public Predictor {
public:
    int n_predict;
    std::string prompt;
    std::string base_api;
    std::string secret;
    std::string grammar;

    int n_threads;
    int req_per_min;
    
private:
    PromptUtil prompt_util;
	unique_ptr<OpenAI> api;

public:
    LlmApiPredictor(std::string prompt, std::string base_api, std::string secret);

public:
    void Config(const ClientConfig &config, const case_insensitive_map_t<Value> &options) override;
    void Load(ClientContext &client, const std::string &path, unique_ptr<PredictStats> &stats) override;
    void PredictChunk(ClientContext &client, DataChunk &input, DataChunk &output, const idx_t rows, const PredictInfo &info, unique_ptr<PredictStats> &stats) override;
    vector<string> PredictString(ClientContext &client, vector<string> &input, const PredictInfo &info) override;
    void ScanChunk(ClientContext &client, DataChunk &output, const PredictInfo &info, unique_ptr<PredictStats> &stats) override;
    std::unique_ptr<BatchResult> PredictBatch(OpenAI& api, DataChunk &input, const idx_t rows, int batch, int batch_size, const PredictInfo &info);
    std::unique_ptr<BatchResult> PredictOne(OpenAI& api, const string &input, const PredictInfo &info);

private:
    void GenerateGrammar();
    std::string GenerateSystemMessage(bool is_array) const; 

};
} // namespace duckdb
