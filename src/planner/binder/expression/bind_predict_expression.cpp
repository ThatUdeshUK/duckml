#include "duckdb/catalog/catalog.hpp"
#include "duckdb/catalog/catalog_entry/model_catalog_entry.hpp"
#include "duckdb/parser/expression/predict_expression.hpp"
#include "duckdb/planner/binder.hpp"
#include "duckdb/planner/expression/bound_predict_expression.hpp"
#include "duckdb/main/secret/secret_manager.hpp"

namespace duckdb {

BindResult ExpressionBinder::BindExpression(PredictExpression &expr, idx_t depth) {
	return BindPredict(expr, depth);
}

BindResult ExpressionBinder::BindPredict(PredictExpression &expr, idx_t depth) {
	if (expr.agg) {
		return BindAggregate(expr, depth);
	}

	// bind the children of the function expression
	ErrorData error;

	// // bind of each child
	for (idx_t i = 0; i < expr.children.size(); i++) {
		BindChild(expr.children[i], depth, error);
	}

	if (error.HasError()) {
		return BindResult(std::move(error));
	}
	
	// all children bound successfully
	// extract the children and types
	vector<LogicalType> child_types;
	vector<unique_ptr<Expression>> children;
	for (idx_t i = 0; i < expr.children.size(); i++) {
		auto &child = BoundExpression::GetExpression(*expr.children[i]);
		child_types.push_back(child->return_type);
		children.push_back(std::move(child));
	}

	auto bound_predict = std::make_unique<BoundPredictInfo>();
	bound_predict->model_name = expr.model_name;
	bound_predict->prompt = expr.prompt;
	bound_predict->input_set_names = std::move(expr.input_col_names);
	bound_predict->input_set_types = std::move(child_types);
	bound_predict->result_set_names.push_back(expr.out_col_name);
	bound_predict->result_set_types.push_back(expr.out_col_type);
											   
	auto result = std::make_unique<BoundPredictExpression>(expr.out_col_type, std::move(children));
	result->bound_predict = std::move(bound_predict);

	vector<reference<ModelCatalogEntry>> entries;
	auto schemas = Catalog::GetAllSchemas(context);
	for (auto &schema : schemas) {
		schema.get().Scan(context, CatalogType::MODEL_ENTRY, [&](CatalogEntry &entry) {
			if (auto &item = entry.Cast<ModelCatalogEntry>(); item.name == result->bound_predict->model_name) {
				entries.push_back(item);
			}
		});
	}

	if (entries.empty()) {
		throw BinderException("Model with name \"%s\" does not exist in calatog!",
				    		   result->bound_predict->model_name.c_str());
	}

	auto &stored_model = entries[0].get();
	auto stored_model_data = stored_model.GetData();

	if (!stored_model_data.secret.empty()) {
		auto &secret_manager = SecretManager::Get(context);
		const auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context);

		if (const auto secret_entry = secret_manager.GetSecretByName(transaction, stored_model_data.secret)) {
			const auto &kv_secret = dynamic_cast<const KeyValueSecret &>(*secret_entry->secret);
			result->bound_predict->secret = kv_secret.TryGetValue("bearer_token").ToString();
		} else {
			throw CatalogException("Secret " + stored_model_data.secret + " for the API is not found in the catalogs!");
		}
	}

	result->bound_predict->model_type = stored_model_data.model_type;
	result->bound_predict->model_path = stored_model_data.model_path;
	result->bound_predict->base_api = stored_model_data.base_api;
	result->bound_predict->options = stored_model_data.options;
	
	if (!result) {
		error.AddQueryLocation(expr);
		error.Throw();
	}

	return BindResult(std::move(result));
}

} // namespace duckdb
