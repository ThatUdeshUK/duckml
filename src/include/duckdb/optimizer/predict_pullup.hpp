//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/optimizer/predict_pullup.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/unordered_set.hpp"
#include "duckdb/planner/logical_operator.hpp"
#include "duckdb/planner/operator/logical_predict.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/planner/tableref/bound_predictref.hpp"

namespace duckdb {

class PredictPullup {
public:
	explicit PredictPullup(bool pullup = false, bool add_column = false)
	    : can_pullup(pullup), can_add_column(add_column) {
	}

	//! Perform filter pullup
	unique_ptr<LogicalOperator> Rewrite(unique_ptr<LogicalOperator> op);

private:
	vector<unique_ptr<BoundPredictInfo>> predict_info_pullup;

	// only pull up filters when there is a fork
	bool can_pullup = false;

	// identifiy case the branch is a set operation (INTERSECT or EXCEPT)
	bool can_add_column = false;

private:
	// Generate logical predict pulled up
	unique_ptr<LogicalOperator> GeneratePullupPredict(unique_ptr<LogicalOperator> child,
	                                                  vector<unique_ptr<BoundPredictInfo>> &predict_infos);

	//! Pull up a LogicalPredict op
	unique_ptr<LogicalOperator> PullupPredict(unique_ptr<LogicalOperator> op);

	//! Pull up predict in a LogicalProjection op
	unique_ptr<LogicalOperator> PullupProjection(unique_ptr<LogicalOperator> op);

	//! Pull up predict in a LogicalCrossProduct op
	unique_ptr<LogicalOperator> PullupCrossProduct(unique_ptr<LogicalOperator> op);

	unique_ptr<LogicalOperator> PullupJoin(unique_ptr<LogicalOperator> op);

	// Pullup predict in a left join
	unique_ptr<LogicalOperator> PullupFromLeft(unique_ptr<LogicalOperator> op);

	// Pullup predict in a inner join
	unique_ptr<LogicalOperator> PullupInnerJoin(unique_ptr<LogicalOperator> op);

	// Pullup predict in LogicalIntersect or LogicalExcept op
	unique_ptr<LogicalOperator> PullupSetOperation(unique_ptr<LogicalOperator> op);

	unique_ptr<LogicalOperator> PullupBothSide(unique_ptr<LogicalOperator> op);

	// Finish pull up at this operator
	unique_ptr<LogicalOperator> FinishPullup(unique_ptr<LogicalOperator> op);

}; // end PredictPullup

} // namespace duckdb
