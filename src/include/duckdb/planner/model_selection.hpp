#pragma once

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/catalog/catalog_entry/model_catalog_entry.hpp"
#include "duckdb/common/exception/binder_exception.hpp"
#include "duckdb/main/client_config.hpp"

#include <random>

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
		default:
			selector = make_uniq<FirstModelSelector>();
			break;
		}
		model_idx = selector->Select(entries, prompt);
	}

    return entries[model_idx].get();
}

} // namespace duckdb