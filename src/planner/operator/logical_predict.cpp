#include "duckdb/planner/operator/logical_predict.hpp"

namespace duckdb {

LogicalPredict::LogicalPredict() : LogicalOperator(LogicalOperatorType::LOGICAL_PREDICT) {
}

LogicalPredict::LogicalPredict(idx_t predict_idx, unique_ptr<BoundPredictInfo> info_p)
    : LogicalOperator(LogicalOperatorType::LOGICAL_PREDICT), predict_index(predict_idx),
      bound_predict(std::move(info_p)) {
}

vector<ColumnBinding> LogicalPredict::GetColumnBindings() {
	return GenerateColumnBindings(predict_index, bound_predict->types.size());
}

vector<idx_t> LogicalPredict::GetTableIndex() const {
	return vector<idx_t> {predict_index};
}

void LogicalPredict::ResolveTypes() {
	this->types = bound_predict->types;
}

string LogicalPredict::GetName() const {
#ifdef DEBUG
	if (DBConfigOptions::debug_print_bindings) {
		return LogicalOperator::GetName() + StringUtil::Format(" #%llu", predict_index);
	}
#endif
	return LogicalOperator::GetName();
}

} // namespace duckdb
