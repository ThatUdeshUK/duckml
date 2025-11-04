#include "duckdb/common/enums/model_type.hpp"
#include "duckdb/common/enum_util.hpp"
#include "duckdb/execution/operator/projection/physical_predict.hpp"
#include "duckdb/execution/physical_plan_generator.hpp"
#include "duckdb/planner/operator/logical_predict.hpp"

namespace duckdb {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalPredict &op) {
	switch (op.bound_predict->model_type) {
	case ModelType::TABULAR:
	case ModelType::LM: {
		D_ASSERT(op.children.size() == 1);
		auto &plan = CreatePlan(*op.children[0]);
		return Make<PhysicalPredict>(std::move(op.types), plan, std::move(op.bound_predict));
	}
	case ModelType::LLM: {
		if (op.children.size() == 0) {
			return Make<PhysicalPredictScan>(std::move(op.types), std::move(op.bound_predict));
		} else if (op.children.size() == 1) {
			auto &plan = CreatePlan(*op.children[0]);
			return Make<PhysicalPredict>(std::move(op.types), plan, std::move(op.bound_predict));
		}
		throw InternalException("Plan Error: Invalid number of child plans");
	}
	// case ModelType::GNN: {
	// 	D_ASSERT(op.children.size() == 2);
	// 	idx_t node_cardinality = op.children[0]->EstimateCardinality(context);
	// 	idx_t edge_cardinality = op.children[1]->EstimateCardinality(context);
	// 	auto &node_plan = CreatePlan(*op.children[0]);
	// 	auto &edge_plan = CreatePlan(*op.children[1]);
	// 	return Make<PhysicalGNNPredict>(std::move(op.types), node_plan, edge_plan, node_cardinality, edge_cardinality, *op.bound_predict.get());
	// }
	default:
		throw InternalException("Plan Error: Unknown model type `" +
		                        std::string(EnumUtil::ToString<ModelType>(op.bound_predict->model_type)));
	}
}

} // namespace duckdb
