#include "duckdb/common/exception.hpp"
#include "duckdb/common/vector_operations/vector_operations.hpp"
#include "duckdb/function/aggregate/distributive_functions.hpp"
#include "duckdb/function/aggregate/distributive_function_utils.hpp"
#include "duckdb/planner/expression/bound_aggregate_expression.hpp"
#include "duckdb/function/aggregate/llm_agg_helpers.hpp"
#include "duckdb/execution/operator/projection/physical_predict.hpp"
#include "duckdb/main/secret/secret_manager.hpp"

#include <iostream>

namespace duckdb {

struct LlmAggState {
	string value;
};

struct LlmAggFunction {
	using STATE = LlmAggState;

	template <class STATE>
	static void Initialize(STATE &state) {
		state.value = "";
	}

	template <class STATE, class OP>
	static void Combine(const STATE &source, STATE &target, AggregateInputData &aggr_input_data) {
		std::cout << "----------- Combine -----------" << std::endl;
		std::cout << "target: " << target.value << " \n" << "source: " << source.value << std::endl;
		std::cout << "-------------------------------" << std::endl;
		target.value += source.value;
	}

	static unique_ptr<Predictor> InitializePredictor(ClientContext &context, PredictInfo &info) {
		std::string api_key;
		if (!info.secret.empty()) {
			auto &secret_manager = SecretManager::Get(context);
			auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context);

			auto secret_entry = secret_manager.GetSecretByName(transaction, info.secret);

			if (secret_entry) {
				const auto &kv_secret = dynamic_cast<const KeyValueSecret &>(*secret_entry->secret);
				api_key = kv_secret.TryGetValue("bearer_token").ToString();
			} else {
				throw CatalogException("Secret for the API is not found in the catalogs!");
			}
		}
		
		auto predictor = PhysicalPredict::InitPredictor(info, api_key);
		predictor->task = PredictorTask::PREDICT_LLM_TASK;
		return std::move(predictor);
	}

	static void Finalize(Vector &states, AggregateInputData &aggr_input_data, Vector &result, idx_t count, idx_t offset) {
		auto &llm_bind = aggr_input_data.bind_data->Cast<LlmAggBindData>();

		PredictInfo predict_info{};
		predict_info.model_type = llm_bind.info->model_type;
		predict_info.model_path = llm_bind.info->model_path;
		predict_info.prompt = llm_bind.info->prompt;
		predict_info.base_api = llm_bind.info->base_api;
		predict_info.secret = llm_bind.info->secret;
		predict_info.result_set_names = llm_bind.info->result_set_names;
		predict_info.input_set_names = llm_bind.info->input_set_names;
		predict_info.result_set_types = llm_bind.info->result_set_types;
		predict_info.options = llm_bind.info->options;

		auto predictor = InitializePredictor(llm_bind.context, predict_info);
		
		auto stats = make_uniq<PredictStats>();
		auto &client_config = ClientConfig::GetConfig(llm_bind.context);
		predictor->Config(client_config, predict_info.options);
		predictor->Load(predict_info.model_path, stats);

		std::cout << "----------- Finalize ----------" << std::endl;
		auto state_data = FlatVector::GetData<STATE *>(states);
		auto results = FlatVector::GetData<string_t>(result);
		for (idx_t i = 0; i < count; i++) {
			auto test = state_data[i];
			auto agg_str = test->value;
			auto emb_str = llm_bind.info->prompt + "\n" + agg_str;

			std::cout << "idx: " << i << " :" << std::endl;
			auto out = predictor->PredictString(llm_bind.context, agg_str, predict_info);
			std::cout << "output: \n" << out << std::endl;
			std::cout << "-------------------------------" << std::endl;
			results[i] = StringVector::AddString(result, out);
		}
	}

	static bool IgnoreNull() {
		return true;
	}

	static void ScatterUpdate(Vector inputs[], AggregateInputData &aggr_input_data, idx_t input_count, Vector &states,
	                          idx_t count) {
		auto &llm_bind = aggr_input_data.bind_data->Cast<LlmAggBindData>();

		auto &input = inputs[0];
		auto input_name = llm_bind.info->input_set_names[0];

		if (input.GetVectorType() == VectorType::FLAT_VECTOR && states.GetVectorType() == VectorType::FLAT_VECTOR) {
			auto states_data = FlatVector::GetData<STATE *>(states);

			UnifiedVectorFormat val_format;
			input.ToUnifiedFormat(count, val_format);

			for (idx_t i = 0; i < count; i++) {
				const auto val_idx = val_format.sel->get_index(i);
				if (!val_format.validity.RowIsValid(val_idx)) {
					continue;
				}
				
				auto value = UnifiedVectorFormat::GetData<string_t>(val_format)[val_idx];
				auto &result = *reinterpret_cast<STATE *>(states_data[i]);

				std::cout << "----------- Update ------------" << std::endl;
				std::cout << "value: " << value.GetString() << " \n" << "state: " << result.value << std::endl;
				std::cout << "-------------------------------" << std::endl;

				result.value += "\n{" + input_name + "=`" + value.GetString() + "`}";
			}
		} else {
			// UnifiedVectorFormat idata, sdata;
			// input.ToUnifiedFormat(count, idata);
			// states.ToUnifiedFormat(count, sdata);
			// CountScatterLoop(reinterpret_cast<STATE **>(sdata.data), *idata.sel, *sdata.sel, idata.validity, count);
		}
	}

	static void Update(Vector inputs[], AggregateInputData &aggr_input_data, idx_t input_count, data_ptr_t state_p, idx_t count) {
		auto &llm_bind = aggr_input_data.bind_data->Cast<LlmAggBindData>();
		auto &result = *reinterpret_cast<STATE *>(state_p);
		
		auto &input = inputs[0];
		auto input_name = llm_bind.info->input_set_names[0];

		UnifiedVectorFormat val_format;
		input.ToUnifiedFormat(count, val_format);

		for (idx_t i = 0; i < count; i++) {
			const auto val_idx = val_format.sel->get_index(i);
			if (!val_format.validity.RowIsValid(val_idx)) {
				continue;
			}
			
			auto &value = UnifiedVectorFormat::GetData<string_t>(val_format)[val_idx];

			std::cout << "----------- Update ------------" << std::endl;
			std::cout << "value: " << value.GetString() << " \n" << "state: " << result.value << std::endl;
			std::cout << "-------------------------------" << std::endl;

			result.value += "\n{" + input_name + "=`" + value.GetString() + "`}";
		}
	}
};

AggregateFunctionSet LlmAggFun::GetFunctions() {
	using STATE = LlmAggState;
	AggregateFunction fun({LogicalType(LogicalTypeId::ANY)}, LogicalType::VARCHAR, AggregateFunction::StateSize<STATE>,
	                      AggregateFunction::StateInitialize<STATE, LlmAggFunction>, LlmAggFunction::ScatterUpdate,
	                      AggregateFunction::StateCombine<STATE, LlmAggFunction>,
	                      LlmAggFunction::Finalize, 
						  FunctionNullHandling::SPECIAL_HANDLING, LlmAggFunction::Update);
	fun.name = "llm_agg";
	fun.order_dependent = AggregateOrderDependent::NOT_ORDER_DEPENDENT;

	AggregateFunctionSet llm_agg("llm_agg");
	llm_agg.AddFunction(fun);
	return llm_agg;
}

} // namespace duckdb
