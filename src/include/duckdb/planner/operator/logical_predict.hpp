//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/planner/operator/logical_predict.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/unordered_map.hpp"
#include "duckdb/common/unordered_set.hpp"
#include "duckdb/planner/logical_operator.hpp"
#include "duckdb/parser/tableref/table_predict_ref.hpp"
#include "duckdb/planner/tableref/bound_predictref.hpp"

namespace duckdb {

class LogicalPredict : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_PREDICT;

public:
	LogicalPredict();
	LogicalPredict(idx_t predict_idx, unique_ptr<BoundPredictInfo> info);

	idx_t predict_index;
	unique_ptr<BoundPredictInfo> bound_predict;

public:
	vector<ColumnBinding> GetColumnBindings() override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

protected:
	void ResolveTypes() override;
};
} // namespace duckdb
