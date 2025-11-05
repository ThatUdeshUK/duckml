#include "duckdb/optimizer/predict_pullup.hpp"

namespace duckdb {

unique_ptr<LogicalOperator> PredictPullup::PullupPredict(unique_ptr<LogicalOperator> op) {
	D_ASSERT(op->type == LogicalOperatorType::LOGICAL_PREDICT);

	auto &filter = op->Cast<LogicalPredict>();
	if (can_pullup) {
		unique_ptr<LogicalOperator> child = std::move(op->children[0]);
		child = Rewrite(std::move(child));
		predict_info_pullup.push_back(std::move(filter.bound_predict));
		return child;
	}
	op->children[0] = Rewrite(std::move(op->children[0]));
	return op;
}

} // namespace duckdb
