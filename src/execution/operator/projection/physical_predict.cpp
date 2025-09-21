#include "duckdb/execution/operator/projection/physical_predict.hpp"

#include "duckdb/main/secret/secret_manager.hpp"
#include "duckdb/common/exception/catalog_exception.hpp"

#include <iostream>
#include <map>

#if defined(ENABLE_PREDICT) && PREDICTOR_IMPL == 2
#include "duckdb_onnx.hpp"
#elif defined(ENABLE_PREDICT) && PREDICTOR_IMPL == 3
#include "duckdb_llama_cpp.hpp"
#endif

#if defined(ENABLE_PREDICT) && defined(ENABLE_LLM_API)
#include "duckdb_llm_api.hpp"
#endif

#define CHUNK_PRED 1
// #define VEC_PRED 0

namespace duckdb {
class PredictState : public OperatorState {
public:
	explicit PredictState(ExecutionContext &context, unique_ptr<Predictor> p, unique_ptr<PredictStats> s)
	    : predictor(std::move(p)), stats(std::move(s)) {
	}

	unique_ptr<Predictor> predictor;
	unique_ptr<PredictStats> stats;

public:
	void Finalize(const PhysicalOperator &op, ExecutionContext &context) override {
#ifdef DEBUG
		std::cout << "Load @run: " << stats->load << std::endl;
		std::cout << "Move @run: " << stats->move << std::endl;
		std::cout << "Predict @run: " << stats->predict << std::endl;
		std::cout << "Move Rev @run: " << stats->move_rev << std::endl;
		std::cout << "Batch Size: " << predictor->batch_size << std::endl;
#endif
		std::map<std::string, long> stats_map {{"load", stats->load},       {"move", stats->move},
		                                       {"predict", stats->predict}, {"move_rev", stats->move_rev},
		                                       {"correct", stats->correct}, {"total", stats->total}};

		context.thread.profiler.Flush(op, stats_map);
	}
};

PhysicalPredict::PhysicalPredict(vector<LogicalType> types_p, PhysicalOperator &child, unique_ptr<BoundPredictInfo> bound_predict_p)
    : PhysicalOperator(PhysicalOperatorType::PREDICT, std::move(types_p), child.estimated_cardinality) {
	children.push_back(child);

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

unique_ptr<Predictor> PhysicalPredict::InitPredictor(const PredictInfo &info, const std::string &api_key) {
#if defined(ENABLE_PREDICT) && PREDICTOR_IMPL == 1
	return make_uniq<TorchPredictor>();
#elif defined(ENABLE_PREDICT) && PREDICTOR_IMPL == 2
	return make_uniq<ONNXPredictor>();
#elif defined(ENABLE_PREDICT) && (PREDICTOR_IMPL == 3 || defined(ENABLE_LLM_API))
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
		return make_uniq<LlamaCppPredictor>(info.prompt);
#else
		throw InternalException("Unable to infer local LLMs without `PREDICTOR_IMPL='llama_cpp'` build option.");
#endif
	}
#else
	return nullptr;
#endif
}

unique_ptr<OperatorState> PhysicalPredict::GetOperatorState(ExecutionContext &context) const {
	auto &client_config = ClientConfig::GetConfig(context.client);

	std::string api_key;
	if (!predict_info.secret.empty()) {
		auto &secret_manager = SecretManager::Get(context.client);
		auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context.client);

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
	return make_uniq<PredictState>(context, std::move(p), std::move(stats));
}

OperatorResultType PhysicalPredict::Execute(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
                                            GlobalOperatorState &gstate, OperatorState &state_p) const {
	auto &state = state_p.Cast<PredictState>();

	DataChunk predictions;
	chunk.Split(predictions, input.ColumnCount());
	input.Copy(chunk);

	predictions.SetCardinality(input);

	auto &predictor = *state.predictor.get();
#if CHUNK_PRED
	if (predictor.task == PREDICT_TABULAR_TASK) {
		predictor.PredictChunk(context.client, input, predictions, (int)input.size(), predict_info, state.stats);
	} else if (predictor.task == PREDICT_LM_TASK) {
		predictor.PredictLMChunk(input, predictions, (int)input.size(), predict_info, state.stats);
	} else if (predictor.task == PREDICT_LLM_TASK) {
		predictor.PredictChunk(context.client, input, predictions, (int)input.size(), predict_info, state.stats);
	}
#elif VEC_PRED
	std::vector<float> inputs;
	std::vector<float> outputs;
	for (idx_t row_idx = 0; row_idx < input.size(); row_idx++) {
		for (idx_t col_idx = 0; col_idx < input.ColumnCount(); col_idx++) {
			inputs.push_back(FloatValue::Get(input.GetValue(col_idx, row_idx)));
		}
	}
	predictor.PredictVector(inputs, outputs, (int)input.size(), (int)input.ColumnCount(), (int)result_set_types.size());
	int col_count = (int)result_set_types.size();
	idx_t idx = 0;
	for (const float &i : outputs) {
		predictions.SetValue(idx % col_count, idx / col_count, Value(i));
		idx++;
	}
	inputs.clear();
	outputs.clear();
#else
	if (predictor.task == PREDICT_TABULAR_TASK) {
		int output_size = (int)result_set_types.size();
		std::vector<float> inputs;
		std::vector<float> outputs;

		for (idx_t row_idx = 0; row_idx < input.size(); row_idx++) {
			for (idx_t col_idx = 0; col_idx < input.ColumnCount(); col_idx++) {
				inputs.push_back(FloatValue::Get(input.GetValue(col_idx, row_idx)));
			}
			predictor.Predict(inputs, outputs, output_size);
			idx_t res_idx = 0;
			for (const float &i : outputs) {
				predictions.SetValue(res_idx, row_idx, Value(i));
				res_idx++;
			}

			inputs.clear();
			outputs.clear();
		}
	} else if (predictor.task == PREDICT_LM_TASK) {
		int output_size = (int)result_set_types.size();
		std::string input_str;
		std::vector<float> outputs;

		for (idx_t row_idx = 0; row_idx < input.size(); row_idx++) {
			input_str = StringValue::Get(input.GetValue(0, row_idx));

			predictor.PredictLM(input_str, outputs, output_size);
			idx_t res_idx = 0;
			for (const float &i : outputs) {
				predictions.SetValue(res_idx, row_idx, Value(i));
				res_idx++;
			}

			outputs.clear();
		}
	}
#endif

	chunk.Fuse(predictions);
	return OperatorResultType::NEED_MORE_INPUT;
}

string PhysicalPredict::GetName() const {
	return "PREDICT";
}

InsertionOrderPreservingMap<string> PhysicalPredict::ParamsToString() const {
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

} // namespace duckdb
