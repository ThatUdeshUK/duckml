#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/planner/expression/bound_predict_expression.hpp"
#include "duckdb/execution/operator/projection/physical_predict.hpp"
#include "duckdb/main/secret/secret_manager.hpp"

#if defined(ENABLE_PREDICT) && PREDICTOR_IMPL == 3
#include "duckdb_llama_cpp.hpp"
#endif

#if defined(ENABLE_PREDICT) && defined(ENABLE_LLM_API)
#include "duckdb_llm_api.hpp"
#endif


namespace duckdb {

class ExecutePredictState : public ExpressionState {
public:
	ExecutePredictState(const Expression &expr, ExpressionExecutorState &root) 
		: ExpressionState(expr, root) {
	};

	~ExecutePredictState() override {
	}

	PredictInfo predict_info;
	unique_ptr<Predictor> predictor;
	unique_ptr<PredictStats> stats;
	bool is_loaded = false;
};

unique_ptr<ExpressionState> ExpressionExecutor::InitializeState(const BoundPredictExpression &expr,
                                                                ExpressionExecutorState &root) {
	auto result = make_uniq<ExecutePredictState>(expr, root);
	idx_t mask_i = 0;
	for (auto &child : expr.children) {
		result->AddChild(*child);
		result->predict_info.input_mask.push_back(mask_i);
		mask_i++;
	}

	result->predict_info.model_type = expr.bound_predict->model_type;
	result->predict_info.model_path = expr.bound_predict->model_path;
	result->predict_info.prompt = expr.bound_predict->prompt;
	result->predict_info.base_api = expr.bound_predict->base_api;
	result->predict_info.secret = expr.bound_predict->secret;
	result->predict_info.result_set_names = expr.bound_predict->result_set_names;
	result->predict_info.input_set_names = expr.bound_predict->input_set_names;
	result->predict_info.result_set_types = expr.bound_predict->result_set_types;
	result->predict_info.options = expr.bound_predict->options;

	std::string api_key;
	if (!result->predict_info.secret.empty()) {
		auto &context = result->GetContext();

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
	
	auto stats = make_uniq<PredictStats>();
	result->stats = std::move(stats);

	result->Finalize();
	return std::move(result);
}

static void VerifyNullHandling(const BoundPredictExpression &expr, DataChunk &args, Vector &result) {
}

void ExpressionExecutor::Execute(const BoundPredictExpression &expr, ExpressionState *state,
                                 const SelectionVector *sel, idx_t count, Vector &result) {
	state->intermediate_chunk.Reset();
	auto &arguments = state->intermediate_chunk;

	DataChunk input;
    input.InitializeEmpty(state->types); // schema
    input.SetCardinality(count);

	if (!state->types.empty()) {
		for (idx_t i = 0; i < expr.children.size(); i++) {
			D_ASSERT(state->types[i] == expr.children[i]->return_type);
			Execute(*expr.children[i], state->child_states[i].get(), sel, count, arguments.data[i]);
			input.data[i].Reference(arguments.data[i]);
// #ifdef DEBUG
// 			if (expr.children[i]->return_type.id() == LogicalTypeId::VARCHAR) {
// 				arguments.data[i].UTFVerify(count);
// 			}
// #endif
		}
	}
	arguments.SetCardinality(count);
	arguments.Verify();

	
	auto &pstate = state->Cast<ExecutePredictState>();
	if (!pstate.is_loaded) {
		auto &client_config = ClientConfig::GetConfig(*context);

		pstate.predictor->Config(client_config, pstate.predict_info.options);
		pstate.predictor->Load(pstate.predict_info.model_path, pstate.stats);
		pstate.is_loaded = true;
	}

	DataChunk predictions;
    predictions.InitializeEmpty({expr.return_type}); // schema
    predictions.SetCardinality(count);
	predictions.data[0].Reference(result);

	pstate.predictor->PredictChunk(*context, input, predictions, count, pstate.predict_info, pstate.stats);

	Vector empty(result.GetType());
	predictions.data[0].Reference(empty);

	VerifyNullHandling(expr, arguments, result);
	D_ASSERT(result.GetType() == expr.return_type);
}

} // namespace duckdb
