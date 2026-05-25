//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/planner/tableref/bound_predictref.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/enums/model_type.hpp"
#include "duckdb/planner/binder.hpp"
#include "duckdb/planner/bound_tableref.hpp"

namespace duckdb {

struct BoundPredictInfo {
	ModelType model_type;
	string model_name;
	//! The path of the model
	string model_path;
	//! The prompt for llm tasks
	string prompt;
	//! The base api for online LLM API
	string base_api;
	//! The base api secret
	string secret;
	//! The set of types
	vector<LogicalType> types;
	//! Input mask for feature column
	vector<idx_t> input_mask;
	//! Input mask for (optional) edges columns for GNN
	vector<idx_t> opt_mask;
	//! Input set names
	vector<string> input_set_names;
	//! Result set names
	vector<string> result_set_names;
	//! Input set types
	vector<LogicalType> input_set_types;
	//! Result set types
	vector<LogicalType> result_set_types;
	//! Options
	case_insensitive_map_t<Value> options;
	//! Mapping from original input column name to its precomputed embedding column name
	case_insensitive_map_t<string> embedding_column_map;

	bool Equals(const BoundPredictInfo &other) const {
		return model_type == other.model_type && model_name == other.model_name &&
		       model_path == other.model_path && prompt == other.prompt;
	}

	void Serialize(Serializer &serializer) const;
	static unique_ptr<BoundPredictInfo> Deserialize(Deserializer &deserializer);
};

class BoundPredictRef : public BoundTableRef {
public:
	static constexpr auto TYPE = TableReferenceType::PREDICT;

public:
	explicit BoundPredictRef() : BoundTableRef(TableReferenceType::PREDICT), bind_index(0) {
	}

	idx_t bind_index;
	//! The binder used to bind the child of the predict
	shared_ptr<Binder> child_binder;
	//! The binder used to bind the optional child for predict
	shared_ptr<Binder> opt_binder;
	//! The child node of the Predict
	vector<unique_ptr<BoundTableRef>> children;
	//! The bound predict info
	unique_ptr<BoundPredictInfo> bound_predict;
};
} // namespace duckdb
