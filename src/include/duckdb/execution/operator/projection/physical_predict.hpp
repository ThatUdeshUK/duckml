//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/execution/operator/projection/physical_predict.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/execution/physical_operator.hpp"
#include "duckdb/main/client_config.hpp"
#include "duckdb/common/common.hpp"
#include "duckdb/common/enums/model_type.hpp"
#include "duckdb/planner/tableref/bound_predictref.hpp"

namespace duckdb {

typedef enum PredictorTask {
	PREDICT_TABULAR_TASK = 0,
	PREDICT_LM_TASK = 1,
	PREDICT_GNN_TASK = 2,
	PREDICT_LLM_TASK = 3
} PredictorTask;

struct PredictStats {
	long load;
	long preprocess;
	long move;
	long predict;
	long move_rev;

	long correct;
	long total;
};

struct PredictInfo {
	ModelType model_type;
	string model_name;
	string model_path;
	string prompt;
	string base_api;
	string secret;

	vector<idx_t> input_mask;
	vector<string> input_set_names;
	vector<string> result_set_names;
	vector<LogicalType> result_set_types;
	case_insensitive_map_t<Value> options;
};

class Predictor {
public:
	explicit Predictor()
	    : task(PREDICT_TABULAR_TASK), batch_size(-1), llm_max_tokens(-1), use_cache(true), use_batch(true),
	      success(false), error_message("") {
	}
	virtual ~Predictor() {
	}

	PredictorTask task;
	std::string model_path;

	int batch_size;
	int llm_max_tokens; // LLM specific
	bool use_cache;
	bool use_batch;

	bool success;
	std::string error_message;

	std::unordered_map<std::string, std::string> cache;

public:
	virtual void Config(const ClientConfig &config, const case_insensitive_map_t<Value> &options) {}
	virtual void Load(ClientContext &context, const std::string &path, unique_ptr<PredictStats> &stats) {}
	virtual void PredictChunk(ClientContext &client, DataChunk &input, DataChunk &output, const idx_t rows,
	                          const PredictInfo &info, unique_ptr<PredictStats> &stats) {}
	virtual void PredictLMChunk(ClientContext &client, DataChunk &input, DataChunk &output, const idx_t rows,
							  const PredictInfo &info, unique_ptr<PredictStats> &stats) {}
	virtual vector<string> PredictString(ClientContext &client, vector<string> &input, const PredictInfo &info) {
		return {};
	}
	virtual void ScanChunk(ClientContext &client, DataChunk &output, const PredictInfo &info,
	                       unique_ptr<PredictStats> &stats) {}
};

//! PhysicalPredict implements the operator physical PREDICT operation
class PhysicalPredict final : public PhysicalOperator {
public:
	PhysicalPredict(vector<LogicalType> types, PhysicalOperator &child, unique_ptr<BoundPredictInfo> bound_predict_p);

	PredictInfo predict_info;

public:
	string GetName() const override;
	InsertionOrderPreservingMap<string> ParamsToString() const override;

public:
	unique_ptr<OperatorState> GetOperatorState(ExecutionContext &context) const override;
	OperatorResultType Execute(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                           GlobalOperatorState &gstate, OperatorState &state_p) const override;

	bool ParallelOperator() const override {
		return false;
	}

public:
	static unique_ptr<Predictor> InitPredictor(const PredictInfo &info, const std::string &api_key = "");
};

//! PhysicalPredict implements the operator physical PREDICT operation
class PhysicalPredictScan final : public PhysicalOperator {
public:
	PhysicalPredictScan(vector<LogicalType> types, unique_ptr<BoundPredictInfo> bound_predict_p);

	PredictInfo predict_info;

public:
	string GetName() const override;
	InsertionOrderPreservingMap<string> ParamsToString() const override;

public:
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context,
	                                                 GlobalSourceState &gstate) const override;
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetData(ExecutionContext &context, DataChunk &chunk, OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

	bool ParallelSource() const override {
		return false;
	}

	ProgressData GetProgress(ClientContext &context, GlobalSourceState &gstate) const override;

public:
	static unique_ptr<Predictor> InitPredictor(const PredictInfo &info, const std::string &api_key = "");
};

} // namespace duckdb
