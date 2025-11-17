//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/optimizer/limit_pushdown.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/constants.hpp"

namespace duckdb {
class LogicalOperator;
class Optimizer;

class LimitToPredictFilterPropagation {
public:
	//! Optimize LIMIT + FILTER (PREDICT) to LIMIT + FILTER (LIMIT + PREDICT)
	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> op);
	//! Whether we can perform the optimization on this operator
	static bool CanOptimize(LogicalOperator &op);
};

} // namespace duckdb
