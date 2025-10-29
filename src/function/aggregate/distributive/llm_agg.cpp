#include "duckdb/common/exception.hpp"
#include "duckdb/common/vector_operations/vector_operations.hpp"
#include "duckdb/function/aggregate/distributive_functions.hpp"
#include "duckdb/function/aggregate/distributive_function_utils.hpp"
#include "duckdb/planner/expression/bound_aggregate_expression.hpp"

namespace duckdb {

struct LlmAggFunction {
	using STATE = string;

	template <class STATE>
	static void Initialize(STATE &state) {
		state = "";
	}

	template <class STATE, class OP>
	static void Combine(const STATE &source, STATE &target, AggregateInputData &) {
		target += source;
	}

	template <class T, class STATE>
	static void Finalize(STATE &state, T &target, AggregateFinalizeData &finalize_data) {
		target = state;
	}

	static bool IgnoreNull() {
		return true;
	}

	static void ScatterUpdate(Vector inputs[], AggregateInputData &aggr_input_data, idx_t input_llm_agg, Vector &states,
	                          idx_t llm_agg) {
		auto &input = inputs[0];
		
	}

	static void Update(Vector inputs[], AggregateInputData &, idx_t input_llm_agg, data_ptr_t state_p, idx_t llm_agg) {
		auto &input = inputs[0];
		auto &result = *reinterpret_cast<STATE *>(state_p);
	}
};

AggregateFunctionSet LlmAggFun::GetFunctions() {
	using STATE = string;
	AggregateFunction fun({LogicalType(LogicalTypeId::ANY)}, LogicalType::VARCHAR, AggregateFunction::StateSize<STATE>,
	                      AggregateFunction::StateInitialize<STATE, LlmAggFunction>, LlmAggFunction::ScatterUpdate,
	                      AggregateFunction::StateCombine<STATE, LlmAggFunction>,
	                      AggregateFunction::StateFinalize<STATE, STATE, LlmAggFunction>, 
						  FunctionNullHandling::SPECIAL_HANDLING, LlmAggFunction::Update);
	fun.name = "llm_agg";
	fun.order_dependent = AggregateOrderDependent::NOT_ORDER_DEPENDENT;

	AggregateFunctionSet llm_agg("llm_agg");
	llm_agg.AddFunction(fun);
	return llm_agg;
}

} // namespace duckdb
