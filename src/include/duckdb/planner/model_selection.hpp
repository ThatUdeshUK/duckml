#pragma once

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/catalog/catalog_entry/model_catalog_entry.hpp"
#include "duckdb/common/exception/binder_exception.hpp"
#include "duckdb/main/client_config.hpp"

#include <random>

#ifdef ENABLE_PREDICT
#include "optimal_selector.hpp"
#endif

namespace duckdb {

class ModelSelector {
public:
	virtual ~ModelSelector() = default;
	virtual int Select(const vector<reference<ModelCatalogEntry>> &entries, const string &prompt) {
		throw BinderException("Model selection not implemented!");
	}
};

class FirstModelSelector : public ModelSelector {
public:
	int Select(const vector<reference<ModelCatalogEntry>> &entries, const string &prompt) override {
		return 0;
	}
};

class RandomModelSelector : public ModelSelector {
public:
	int Select(const vector<reference<ModelCatalogEntry>> &entries, const string &prompt) override {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(0, static_cast<int>(entries.size()) - 1);
		return dist(gen);
	}
};

#ifdef ENABLE_PREDICT
// Selects the highest-scoring model using the palimpzest get_optimal_models algorithm:
// min-max normalised weighted scoring of quality (MMLU-Pro), cost, and latency.
class OptimalModelSelector : public ModelSelector {
public:
	explicit OptimalModelSelector(ScoringPolicy policy) : policy_(std::move(policy)) {
	}

	int Select(const vector<reference<ModelCatalogEntry>> &entries, const string &prompt) override {
		std::vector<std::string> paths;
		paths.reserve(entries.size());
		for (auto &e : entries) {
			paths.push_back(e.get().GetData().model_path);
		}
		int idx = SelectOptimalModel(paths, policy_);
		return (idx < 0) ? 0 : idx;
	}

private:
	ScoringPolicy policy_;
};
#endif // ENABLE_PREDICT

inline ModelCatalogEntry &SelectModelFromCatalog(ClientContext &context, const string &prompt, const string &model_name) {
	vector<reference<ModelCatalogEntry>> entries;
	int model_idx = -1;
	int model_count = 0;

	auto schemas = Catalog::GetAllSchemas(context);
	for (auto &schema : schemas) {
		schema.get().Scan(context, CatalogType::MODEL_ENTRY, [&](CatalogEntry &entry) {
			auto &item = entry.Cast<ModelCatalogEntry>();
			if (item.name == model_name) {
				model_idx = model_count;
			}
			entries.push_back(item);
			model_count++;
		});
	}

	if (!model_name.empty()) {
		if (model_idx == -1) {
			throw BinderException("Model with name \"%s\" does not exist in calatog!", model_name.c_str());
		}
	} else {
		if (model_count == 0) {
			throw BinderException("No models available in catalog for model selection!");
		}

		auto &config = ClientConfig::GetConfig(context);
		unique_ptr<ModelSelector> selector;
		switch (config.model_select_strategy) {
		case ModelSelectStrategy::RANDOM:
			selector = make_uniq<RandomModelSelector>();
			break;
#ifdef ENABLE_PREDICT
		case ModelSelectStrategy::OPTIMAL: {
			ScoringPolicy policy;
			policy.quality_weight = config.model_select_quality_weight;
			policy.cost_weight    = config.model_select_cost_weight;
			policy.time_weight    = config.model_select_time_weight;
			policy.min_quality    = config.model_select_min_quality;
			policy.max_cost       = config.model_select_max_cost;
			policy.max_time       = config.model_select_max_time;
			selector = make_uniq<OptimalModelSelector>(policy);
			break;
		}
#endif
		default:
			selector = make_uniq<FirstModelSelector>();
			break;
		}
		model_idx = selector->Select(entries, prompt);
	}

    return entries[model_idx].get();
}

} // namespace duckdb