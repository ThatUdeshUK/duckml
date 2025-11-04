#pragma once

#include <onnxruntime_cxx_api.h>

#include <string>
#include <map>
#include "duckdb/common/types/vector.hpp"
#include "duckdb/execution/operator/projection/physical_predict.hpp"
#include <tokenizer.hpp>

#define ROW_FIRST_SET  0
#define COL_FIRST_SET  1
#define ROW_FIRST_COPY 2
#define COL_FIRST_COPY 3
#define ROW_FIRST_PUSH 4

#define MOVE_METHOD     COL_FIRST_COPY
#define MOVE_REV_METHOD COL_FIRST_COPY

#define DYNAMIC_TOKEN_SIZE 1

#define OPT_TIMING 1

namespace duckdb {
class ONNXPredictor final : public Predictor {
public:
	ONNXPredictor();

	int execution_mode;
	int intra_tc;
	int inter_tc;

public:
	void Config(const ClientConfig &config, const case_insensitive_map_t<Value> &options) override;
	void Load(ClientContext &client, const std::string &model_path, unique_ptr<PredictStats> &stats) override;
	void PredictChunk(ClientContext &context, DataChunk &input, DataChunk &output, idx_t rows, const PredictInfo &info,
	                  unique_ptr<PredictStats> &stats) override;
	void PredictLMChunk(ClientContext &context, DataChunk &input, DataChunk &output, idx_t rows,
	                    const PredictInfo &info, unique_ptr<PredictStats> &stats) override;

private:
	Ort::Session session {nullptr};
	FullTokenizer tokenizer;

	void Preprocess(const std::string &text, int64_t *input_ids, int64_t *mask, int offset, int max_length) const;
	void Preprocess2(const std::vector<long> &ids, int64_t *input_ids, int64_t *mask, int offset,
	                 int max_length) const;
};
} // namespace duckdb
