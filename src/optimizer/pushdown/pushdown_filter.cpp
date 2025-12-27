#include "duckdb/optimizer/filter_pushdown.hpp"
#include "duckdb/planner/operator/logical_empty_result.hpp"
#include "duckdb/planner/operator/logical_filter.hpp"
#include "duckdb/planner/expression/bound_comparison_expression.hpp"

namespace duckdb {

using Filter = FilterPushdown::Filter;

unique_ptr<LogicalOperator> FilterPushdown::PushdownFilter(unique_ptr<LogicalOperator> op) {
	D_ASSERT(op->type == LogicalOperatorType::LOGICAL_FILTER);
	auto &filter = op->Cast<LogicalFilter>();
	if (filter.HasProjectionMap()) {
		return FinishPushdown(std::move(op));
	}
	// filter: gather the filters and remove the filter from the set of operations
	vector<unique_ptr<Expression>> predict_expr;
	for (auto &expression : filter.expressions) {
		if (expression->GetExpressionClass() == ExpressionClass::BOUND_COMPARISON) {
			auto &cmpr = expression->Cast<BoundComparisonExpression>();
			if (cmpr.left->type == ExpressionType::PREDICT || cmpr.right->type == ExpressionType::PREDICT) {
				auto &client_config = ClientConfig::GetConfig(GetContext());
				if (client_config.pull_predict_filter) {
					predict_expr.push_back(std::move(expression));
					continue;
				}
			}
		} else if (expression->GetExpressionClass() == ExpressionClass::PREDICT) {
			auto &client_config = ClientConfig::GetConfig(GetContext());
			if (client_config.pull_predict_filter) {
				predict_expr.push_back(std::move(expression));
				continue;
			}
		}
		if (AddFilter(std::move(expression)) == FilterResult::UNSATISFIABLE) {
			// filter statically evaluates to false, strip tree
			return make_uniq<LogicalEmptyResult>(std::move(op));
		}
	}
	GenerateFilters();
	
	if (predict_expr.size() > 0) {
		auto child = Rewrite(std::move(filter.children[0]));
		return AddLogicalFilter(std::move(child), std::move(predict_expr));
	}
	return Rewrite(std::move(filter.children[0]));
}

} // namespace duckdb
