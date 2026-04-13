//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/optimizer/split_predict_join_to_extract_predicate.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/constants.hpp"
#include "duckdb/optimizer/column_binding_replacer.hpp"
#include "duckdb/planner/column_binding_map.hpp"
#include "duckdb/planner/logical_operator.hpp"

namespace duckdb {
class Optimizer;

//! If a LogicalFilter sits directly on top of a join and its expressions contain
//! a PREDICT call whose input columns span *both* sides of the join, this rule
//! rewrites the plan into:
//!
//!   LogicalFilter  [predicate referencing projected PREDICT output col]
//!     └─ LogicalProjection2  [all join cols  +  PREDICT output as extra col]
//!          └─ LogicalProjection1  [all join cols  (identity pass-through)]
//!               └─ <original join>
//!
//! Column bindings in parent operators are updated via ColumnBindingReplacer so
//! that the rest of the plan remains consistent.
class SplitPredictJoinToExtractPredicate {
public:
	explicit SplitPredictJoinToExtractPredicate(Optimizer &optimizer);

	//! Entry point: transform the full plan and fix parent column bindings.
	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> plan);

	//! Returns true when op is a LogicalFilter whose direct child is a supported
	//! join type and at least one filter expression contains a PREDICT sub-expression
	//! whose input columns originate from *both* sides of that join.
	static bool CanOptimize(LogicalOperator &op);

private:
	// One pending binding-fix entry per transformed filter node.
	struct BindingFix {
		//! The filter operator (used as stop_operator for ColumnBindingReplacer).
		LogicalOperator *filter;
		//! Parallel vectors: old_bindings[i] was the join output binding that is
		//! now represented by new_bindings[i] in Projection2.
		vector<ColumnBinding> old_bindings;
		vector<ColumnBinding> new_bindings;
	};

	Optimizer &optimizer;
	vector<BindingFix> pending_fixes;

	//! Recursive top-down pass that transforms matching filter+join nodes.
	unique_ptr<LogicalOperator> Transform(unique_ptr<LogicalOperator> op);

	//! Perform the actual rewrite for a single matching filter+join node.
	unique_ptr<LogicalOperator> TransformFilterJoin(unique_ptr<LogicalOperator> op);
};

} // namespace duckdb