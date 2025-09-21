#include "duckdb/execution/operator/projection/physical_predict.hpp"

#include "duckdb/main/secret/secret_manager.hpp"

#include <iostream>
#include <map>

#if defined(ENABLE_PREDICT) && PREDICTOR_IMPL == 3
#include "duckdb_llama_cpp.hpp"
#endif

#if defined(ENABLE_PREDICT) && defined(ENABLE_LLM_API)
#include "duckdb_llm_api.hpp"
#endif

#define CHUNK_PRED 1
// #define VEC_PRED 0

namespace duckdb {
class PredictScanGlobalState : public GlobalSourceState {
public:
	explicit PredictScanGlobalState(ClientContext &context, unique_ptr<Predictor> p, unique_ptr<PredictStats> s)
	    : predictor(std::move(p)), stats(std::move(s)) {
	}

	unique_ptr<Predictor> predictor;
	unique_ptr<PredictStats> stats;
};

class PredictScanLocalState : public LocalSourceState {
public:
	PredictScanLocalState(ExecutionContext &context) {}
};

PhysicalPredictScan::PhysicalPredictScan(vector<LogicalType> types_p, unique_ptr<BoundPredictInfo> bound_predict_p)
    : PhysicalOperator(PhysicalOperatorType::PREDICT, std::move(types_p), 0) {
	predict_info.model_type = bound_predict_p->model_type;
	predict_info.model_path = std::move(bound_predict_p->model_path);
	predict_info.prompt = std::move(bound_predict_p->prompt);
	predict_info.base_api = std::move(bound_predict_p->base_api);
	predict_info.secret = std::move(bound_predict_p->secret);
	predict_info.input_mask = std::move(bound_predict_p->input_mask);
	predict_info.result_set_names = std::move(bound_predict_p->result_set_names);
	predict_info.input_set_names = std::move(bound_predict_p->input_set_names);
	predict_info.result_set_types = std::move(bound_predict_p->result_set_types);
	predict_info.options = std::move(bound_predict_p->options);
}

unique_ptr<Predictor> PhysicalPredictScan::InitPredictor(const PredictInfo &info, const std::string &api_key) {
#if defined(ENABLE_PREDICT) && (PREDICTOR_IMPL == 3 || defined(ENABLE_LLM_API))
	bool is_api = !(info.model_path.find(".gguf") != std::string::npos);
	std::cout << "Is API Model: " << is_api << std::endl;
	if (is_api) {
#if defined(ENABLE_LLM_API)
		return make_uniq<LlmApiPredictor>(info.prompt, info.base_api, api_key);
#else
		throw InternalException("Unable to infer LLM API model without `ENABLE_LLM_API` build option.");
#endif
	} else {
#if (PREDICTOR_IMPL == 3)
		auto pred = make_uniq<LlamaCppPredictor>(info.prompt);
		pred->is_source = true;
		return std::move(pred);
#else
		throw InternalException("Unable to infer local LLMs without `PREDICTOR_IMPL='llama_cpp'` build option.");
#endif
	}
#else
	return nullptr;
#endif
}

unique_ptr<LocalSourceState> PhysicalPredictScan::GetLocalSourceState(ExecutionContext &context,
                                                                    GlobalSourceState &gstate) const {
	return make_uniq<PredictScanLocalState>(context);
}

unique_ptr<GlobalSourceState> PhysicalPredictScan::GetGlobalSourceState(ClientContext &context) const {
	auto &client_config = ClientConfig::GetConfig(context);

	std::string api_key;
	if (!predict_info.secret.empty()) {
		auto &secret_manager = SecretManager::Get(context);
		auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context);

		auto secret_entry = secret_manager.GetSecretByName(transaction, predict_info.secret);

		if (secret_entry) {
			const auto &kv_secret = dynamic_cast<const KeyValueSecret &>(*secret_entry->secret);
			api_key = kv_secret.TryGetValue("bearer_token").ToString();
		} else {
			throw CatalogException("Secret for the API is not found in the catalogs!");
		}
	}

	auto stats = make_uniq<PredictStats>();
	auto p = InitPredictor(predict_info, api_key);
	p->task = static_cast<PredictorTask>(predict_info.model_type);
	p->Config(client_config, predict_info.options);
	p->Load(predict_info.model_path, stats);

	return make_uniq<PredictScanGlobalState>(context, std::move(p), std::move(stats));
}

SourceResultType PhysicalPredictScan::GetData(ExecutionContext &context, DataChunk &chunk,
                                          OperatorSourceInput &input) const {
	auto &g_state = input.global_state.Cast<PredictScanGlobalState>();
	auto &l_state = input.local_state.Cast<PredictScanLocalState>();

	// predictions.SetCardinality(input);

	auto &predictor = *g_state.predictor.get();
	predictor.ScanChunk(context.client, chunk, predict_info, g_state.stats);
	
	// return chunk.size() == 0 ? SourceResultType::FINISHED : SourceResultType::HAVE_MORE_OUTPUT;
	return SourceResultType::FINISHED;
}

string PhysicalPredictScan::GetName() const {
	return "LLM_SCAN";
}

InsertionOrderPreservingMap<string> PhysicalPredictScan::ParamsToString() const {
	InsertionOrderPreservingMap<string> result;
	result["Type"] = EnumUtil::ToChars<ModelType>(predict_info.model_type);
	result["Model Path"] = predict_info.model_path;

	for (const auto &item : predict_info.options) {
		stringstream ss;
		ss << item.second;
		result[item.first] = ss.str();
	}

	SetEstimatedCardinality(result, estimated_cardinality);
	return std::move(result);
}

ProgressData PhysicalPredictScan::GetProgress(ClientContext &context, GlobalSourceState &gstate_p) const {
	auto &gstate = gstate_p.Cast<PredictScanGlobalState>();
	ProgressData res;
	res.SetInvalid();
	return res;
}

} // namespace duckdb
