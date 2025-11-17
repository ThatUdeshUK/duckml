#include "duckdb/optimizer/limit_to_predict_filter_propagation.hpp"

#include "duckdb/planner/operator/logical_limit.hpp"
#include "duckdb/planner/operator/logical_filter.hpp"
#include "duckdb/planner/expression/bound_comparison_expression.hpp"

namespace duckdb {

bool LimitToPredictFilterPropagation::CanOptimize(duckdb::LogicalOperator &op) {
	if (op.type == LogicalOperatorType::LOGICAL_LIMIT &&
	    op.children[0]->type == LogicalOperatorType::LOGICAL_FILTER) {
		auto &limit = op.Cast<LogicalLimit>();

		if (limit.offset_val.Type() == LimitNodeType::EXPRESSION_PERCENTAGE ||
		    limit.offset_val.Type() == LimitNodeType::EXPRESSION_VALUE) {
			// Offset cannot be an expression
			return false;
		}

		bool hasPredict = false;
		const auto &filter = limit.children[0]->Cast<LogicalFilter>();
		for (const auto &expression : filter.expressions) {
			if (expression->expression_class == ExpressionClass::BOUND_COMPARISON) {
				const auto &comp = expression->Cast<BoundComparisonExpression>();
				if (comp.left->type == ExpressionType::PREDICT || comp.right->type == ExpressionType::PREDICT) {
					hasPredict = true;
				}
			}
		}

		if (hasPredict && limit.limit_val.Type() == LimitNodeType::CONSTANT_VALUE && limit.limit_val.GetConstantValue() < 2048) {
			// Push down only when limit value is smaller than 8192.
			// when physical_limit is introduced, it will end a parallel pipeline
			// restrict the limit value to be small so that remaining operations run fast without parallelization.
			return true;
		}
	}
	return false;
}

unique_ptr<LogicalOperator> LimitToPredictFilterPropagation::Optimize(unique_ptr<LogicalOperator> op) {
	if (CanOptimize(*op)) {
		auto &limit = op->Cast<LogicalLimit>();
		auto &filter = limit.children[0]->Cast<LogicalFilter>();
		filter.SetEstimatedCardinality(limit.estimated_cardinality);
		filter.limit = limit.estimated_cardinality;
	}
	for (auto &child : op->children) {
		child = Optimize(std::move(child));
	}
	return op;
}

} // namespace duckdb
