#include "duckdb/common/pair.hpp"
#include "duckdb/common/types/list_segment.hpp"
#include "core_functions/aggregate/llm_functions.hpp"
#include "duckdb/planner/expression/bound_aggregate_expression.hpp"
#include "duckdb/planner/expression/bound_constant_expression.hpp"
#include "duckdb/execution/operator/projection/physical_predict.hpp"
#include "duckdb/main/secret/secret_manager.hpp"

#include <iostream>

namespace duckdb {

struct LlmAggBindData : public FunctionData {
	explicit LlmAggBindData(const LogicalType &stype_p, string prompt_p);
	~LlmAggBindData() override;

	string prompt;
	LogicalType stype;
	ListSegmentFunctions functions;

	PredictInfo predict_info;
	unique_ptr<Predictor> predictor;

	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<LlmAggBindData>(stype, prompt);
	}

	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<LlmAggBindData>();
		return stype == other.stype && prompt == other.prompt;
	}
};

LlmAggBindData::LlmAggBindData(const LogicalType &stype_p, string prompt_p) : stype(stype_p), prompt(std::move(prompt_p)) {
	// always unnest once because the result vector is of type LlmAgg
	GetSegmentDataFunctions(functions, stype_p);
}

LlmAggBindData::~LlmAggBindData() {
}

struct LlmAggState {
	LinkedList linked_list;
};

struct LlmAggFunction {
	template <class STATE>
	static void Initialize(STATE &state) {
		state.linked_list.total_capacity = 0;
		state.linked_list.first_segment = nullptr;
		state.linked_list.last_segment = nullptr;
	}
	static bool IgnoreNull() {
		return false;
	}
};

static void LlmAggUpdateFunction(Vector inputs[], AggregateInputData &aggr_input_data, idx_t input_count,
                               Vector &state_vector, idx_t count) {

	D_ASSERT(input_count == 2);
	auto &input = inputs[1];
	RecursiveUnifiedVectorFormat input_data;
	Vector::RecursiveToUnifiedFormat(input, count, input_data);

	UnifiedVectorFormat states_data;
	state_vector.ToUnifiedFormat(count, states_data);
	auto states = UnifiedVectorFormat::GetData<LlmAggState *>(states_data);

	auto &list_bind_data = aggr_input_data.bind_data->Cast<LlmAggBindData>();

	for (idx_t i = 0; i < count; i++) {
		auto &state = *states[states_data.sel->get_index(i)];
		aggr_input_data.allocator.AlignNext();
		list_bind_data.functions.AppendRow(aggr_input_data.allocator, state.linked_list, input_data, i);
	}
}

static void LlmAggAbsorbFunction(Vector &states_vector, Vector &combined, AggregateInputData &aggr_input_data,
                               idx_t count) {
	D_ASSERT(aggr_input_data.combine_type == AggregateCombineType::ALLOW_DESTRUCTIVE);

	UnifiedVectorFormat states_data;
	states_vector.ToUnifiedFormat(count, states_data);
	auto states_ptr = UnifiedVectorFormat::GetData<LlmAggState *>(states_data);

	auto combined_ptr = FlatVector::GetData<LlmAggState *>(combined);
	for (idx_t i = 0; i < count; i++) {

		auto &state = *states_ptr[states_data.sel->get_index(i)];
		if (state.linked_list.total_capacity == 0) {
			// NULL, no need to append
			// this can happen when adding a FILTER to the grouping, e.g.,
			// LlmAgg(i) FILTER (WHERE i <> 3)
			continue;
		}

		if (combined_ptr[i]->linked_list.total_capacity == 0) {
			combined_ptr[i]->linked_list = state.linked_list;
			continue;
		}

		// append the linked LlmAgg
		combined_ptr[i]->linked_list.last_segment->next = state.linked_list.first_segment;
		combined_ptr[i]->linked_list.last_segment = state.linked_list.last_segment;
		combined_ptr[i]->linked_list.total_capacity += state.linked_list.total_capacity;
	}
}

static void ListToStringVector(Vector &list_vec, idx_t count, Vector &result) {
    auto list_entries = FlatVector::GetData<list_entry_t>(list_vec);
    auto &child_vec = ListVector::GetEntry(list_vec);      // child elements

    // result is a VARCHAR vector
    auto result_data = FlatVector::GetData<string_t>(result);

    for (idx_t row = 0; row < count; row++) {
        if (FlatVector::IsNull(list_vec, row)) {
            FlatVector::SetNull(result, row, true);
            continue;
        }

        auto entry = list_entries[row];
        string buffer;

        for (idx_t i = 0; i < entry.length; i++) {
            auto idx = entry.offset + i;

            // fetch the child element as a Value (slow, but type-safe).
            // For more efficiency, specialize per child type.
            Value v = child_vec.GetValue(idx);
            if (i > 0) {
                buffer += ",";
            }
            buffer += v.ToString();
        }

        // write into result vector
        result_data[row] = StringVector::AddString(result, buffer);
    }
}

static void LlmAggFinalize(Vector &states_vector, AggregateInputData &aggr_input_data, Vector &result, idx_t count,
                         idx_t offset) {

	UnifiedVectorFormat states_data;
	states_vector.ToUnifiedFormat(count, states_data);
	auto states = UnifiedVectorFormat::GetData<LlmAggState *>(states_data);

	D_ASSERT(result.GetType().id() == LogicalTypeId::VARCHAR);

	LogicalType list_type = LogicalType::LIST(LogicalType::VARCHAR);
	Vector list_vector(list_type);

	auto &mask = FlatVector::Validity(list_vector);
	auto result_data = FlatVector::GetData<list_entry_t>(list_vector);
	size_t total_len = ListVector::GetListSize(list_vector);

	auto &list_bind_data = aggr_input_data.bind_data->Cast<LlmAggBindData>();

	// first iterate over all entries and set up the list entries, and get the newly required total length
	for (idx_t i = 0; i < count; i++) {
		auto &state = *states[states_data.sel->get_index(i)];
		const auto rid = i + offset;
		result_data[rid].offset = total_len;
		if (state.linked_list.total_capacity == 0) {
			mask.SetInvalid(rid);
			result_data[rid].length = 0;
			continue;
		}

		// set the length and offset of this list in the result vector
		auto total_capacity = state.linked_list.total_capacity;
		result_data[rid].length = total_capacity;
		total_len += total_capacity;
	}

	// reserve capacity, then iterate over all entries again and copy over the data to the child vector
	ListVector::Reserve(list_vector, total_len);
	auto &result_child = ListVector::GetEntry(list_vector);
	for (idx_t i = 0; i < count; i++) {

		auto &state = *states[states_data.sel->get_index(i)];
		const auto rid = i + offset;
		if (state.linked_list.total_capacity == 0) {
			continue;
		}

		idx_t current_offset = result_data[rid].offset;
		list_bind_data.functions.BuildListVector(state.linked_list, result_child, current_offset);
	}

	ListVector::SetListSize(list_vector, total_len);
	ListToStringVector(list_vector, count, result);
}

static void LlmAggCombineFunction(Vector &states_vector, Vector &combined, AggregateInputData &aggr_input_data,
                                idx_t count) {

	//	Can we use destructive combining?
	if (aggr_input_data.combine_type == AggregateCombineType::ALLOW_DESTRUCTIVE) {
		LlmAggAbsorbFunction(states_vector, combined, aggr_input_data, count);
		return;
	}

	UnifiedVectorFormat states_data;
	states_vector.ToUnifiedFormat(count, states_data);
	auto states_ptr = UnifiedVectorFormat::GetData<const LlmAggState *>(states_data);
	auto combined_ptr = FlatVector::GetData<LlmAggState *>(combined);

	auto &list_bind_data = aggr_input_data.bind_data->Cast<LlmAggBindData>();
	auto result_type = list_bind_data.stype;

	for (idx_t i = 0; i < count; i++) {
		auto &source = *states_ptr[states_data.sel->get_index(i)];
		auto &target = *combined_ptr[i];

		const auto entry_count = source.linked_list.total_capacity;
		Vector input(result_type, source.linked_list.total_capacity);
		list_bind_data.functions.BuildListVector(source.linked_list, input, 0);

		RecursiveUnifiedVectorFormat input_data;
		Vector::RecursiveToUnifiedFormat(input, entry_count, input_data);

		for (idx_t entry_idx = 0; entry_idx < entry_count; ++entry_idx) {
			aggr_input_data.allocator.AlignNext();
			list_bind_data.functions.AppendRow(aggr_input_data.allocator, target.linked_list, input_data, entry_idx);
		}
	}
}

unique_ptr<FunctionData> LlmAggBindFunction(ClientContext &context, AggregateFunction &function,
                                          vector<unique_ptr<Expression>> &arguments) {
	D_ASSERT(arguments.size() == 2);
	D_ASSERT(function.arguments.size() == 2);

	auto &prompt_arg = arguments[0];
    if (prompt_arg->GetExpressionClass() != ExpressionClass::BOUND_CONSTANT) {
        throw BinderException("First argument to llmAgg must be a constant string");
    }
	auto &const_expr = prompt_arg->Cast<BoundConstantExpression>();
    if (const_expr.value.type().id() != LogicalTypeId::VARCHAR) {
        throw BinderException("First argument to llmAgg must be a string literal");
    }

    auto prompt = StringValue::Get(const_expr.value);
	std::cout << "Prompt: " << prompt << std::endl;

	if (arguments[1]->return_type.id() == LogicalTypeId::UNKNOWN) {
		function.arguments[1] = LogicalTypeId::UNKNOWN;
		function.return_type = LogicalType::SQLNULL;
		return nullptr;
	}

	// Bind the predict

	function.return_type = arguments[1]->return_type;
	auto result = make_uniq<LlmAggBindData>(function.return_type, prompt);
	result->predict_info.prompt = prompt;
	std::string api_key;
	if (!result->predict_info.secret.empty()) {
		auto &secret_manager = SecretManager::Get(context);
		auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context);

		auto secret_entry = secret_manager.GetSecretByName(transaction, result->predict_info.secret);

		if (secret_entry) {
			const auto &kv_secret = dynamic_cast<const KeyValueSecret &>(*secret_entry->secret);
			api_key = kv_secret.TryGetValue("bearer_token").ToString();
		} else {
			throw CatalogException("Secret for the API is not found in the catalogs!");
		}
	}
	auto predictor = PhysicalPredict::InitPredictor(result->predict_info, api_key);
	predictor->task = PredictorTask::PREDICT_LLM_TASK;
	result->predictor = std::move(predictor);
	return std::move(result);
}

AggregateFunction LlmAggFun::GetFunction() {
	auto func =
	    AggregateFunction({LogicalType::VARCHAR, LogicalType::ANY}, LogicalTypeId::VARCHAR, AggregateFunction::StateSize<LlmAggState>,
	                      AggregateFunction::StateInitialize<LlmAggState, LlmAggFunction>, LlmAggUpdateFunction,
	                      LlmAggCombineFunction, LlmAggFinalize, nullptr, LlmAggBindFunction, nullptr, nullptr, nullptr);

	return func;
}

} // namespace duckdb
