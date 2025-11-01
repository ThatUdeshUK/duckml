#pragma once

#include <string>
#include <memory>
#include <map>
#include "duckdb/common/types/data_chunk.hpp"
#include "duckdb/common/types/vector.hpp"
#include "duckdb/execution/operator/projection/physical_predict.hpp"
#include "duckdb/common/string_util.hpp"
#include "../../common/common.hpp"

#include "openai.hpp"

#define OPT_TIMING 1

namespace duckdb {

struct BatchResult {
    size_t tokens;
    long n_calls;
	long move;
	long predict;
	long move_rev;
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

public:
    LlmApiPredictor(std::string prompt, std::string base_api, std::string secret);

public:
    void Config(const ClientConfig &config, const case_insensitive_map_t<Value> &options) override;
    void Load(const std::string &model_path, unique_ptr<PredictStats> &stats) override;
    void PredictChunk(ClientContext &client, DataChunk &input, DataChunk &output, int rows, const PredictInfo &info, unique_ptr<PredictStats> &stats) override;
    vector<string> PredictString(ClientContext &client, vector<string> &input, const PredictInfo &info) override;
    void ScanChunk(ClientContext &client, DataChunk &output, const PredictInfo &info, unique_ptr<PredictStats> &stats) override;
    std::unique_ptr<BatchResult> PredictBatch(ClientContext &client, openai::OpenAI& api, DataChunk &input, int rows, int batch, int batch_size, const PredictInfo &info);
    std::unique_ptr<BatchResult> PredictOne(ClientContext &client, openai::OpenAI& api, string input, const PredictInfo &info);

private:
    void GenerateGrammar();
    std::string GenerateSystemMessage(bool is_array) const; 

};
} // namespace duckdb
