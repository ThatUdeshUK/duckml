#include "duckdb/parser/tableref/table_predict_ref.hpp"
#include "duckdb/common/prompt.hpp"
#include "duckdb/planner/binder.hpp"
#include "duckdb/planner/tableref/bound_predictref.hpp"
#include "duckdb/parser/tableref/subqueryref.hpp"
#include "duckdb/parser/query_node/select_node.hpp"
#include "duckdb/catalog/catalog.hpp"
#include "duckdb/catalog/catalog_entry/model_catalog_entry.hpp"

#include <regex>
#include <iostream>
#include <algorithm>

namespace duckdb {

struct BindModelData {
	vector<reference<ModelCatalogEntry>> entries;
};

unique_ptr<BoundTableRef> Binder::BindBoundPredict(TablePredictRef &ref) {
	auto result = make_uniq<BoundPredictRef>();
	auto bound_predict = make_uniq<BoundPredictInfo>();
	bound_predict->model_name = std::move(ref.model_name);

	bound_predict->prompt = std::move(ref.prompt);

	auto models = make_uniq<BindModelData>();
	auto schemas = Catalog::GetAllSchemas(context);
	for (auto &schema : schemas) {
		schema.get().Scan(context, CatalogType::MODEL_ENTRY, [&](CatalogEntry &entry) {
			if (auto &item = entry.Cast<ModelCatalogEntry>(); item.name == bound_predict->model_name) {
				models->entries.push_back(item);
			}
		});
	}

	if (models->entries.empty()) {
		throw InternalException("Catalog Error: Model with name `" + bound_predict->model_name + "` does not exist!");
	}

	auto &stored_model = models->entries[0].get();
	auto stored_model_data = stored_model.GetData();
	bound_predict->model_type = stored_model_data.model_type;
	bound_predict->model_path = stored_model_data.model_path;
	bound_predict->options = stored_model_data.options;
	if (stored_model_data.on_prompt) {
		bound_predict->base_api = stored_model_data.base_api;
		bound_predict->secret = stored_model_data.secret;

		// Infer input output columns from the PROMPT
		const std::regex out_re(Prompt::OUT_REGEX, std::regex_constants::icase);
		auto words_begin = std::sregex_iterator(bound_predict->prompt.begin(), bound_predict->prompt.end(), out_re);
		auto words_end = std::sregex_iterator();

		for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
			std::smatch match = *i;
			stored_model_data.out_names.push_back(match[1]);

			std::string type = match[2].str();
			LogicalType out_type = Prompt::type_to_logical_type(type);
			stored_model_data.out_types.push_back(out_type);
		}

		for (auto &expr : ref.parsed_input_columns) {
			if (expr->GetExpressionType() == ExpressionType::COLUMN_REF) {
				auto &child_colref = expr->Cast<ColumnRefExpression>();
				if (child_colref.IsQualified()) {
					throw BinderException(*expr, "PREDICT expression cannot contain qualified columns");
				}
				stored_model_data.input_set_names.push_back(child_colref.GetColumnName());
			}
		}
	}

	result->bind_index = GenerateTableIndex();

	vector<string> names;
	vector<LogicalType> input_types;
	vector<LogicalType> types;
	if (ref.source) {
		result->child_binder = CreateBinder(context, this);
		result->children.push_back(result->child_binder->Bind(*ref.source));

		result->child_binder->bind_context.GetTypesAndNames(names, input_types);
		types.insert(types.end(), input_types.begin(), input_types.end());

		vector<idx_t> input_mask;
		if (!stored_model_data.input_set_names.empty()) {
			case_insensitive_map_t<idx_t> name_map;
			for (auto it = names.begin(); it != names.end(); ++it) {
				auto index = static_cast<idx_t>(std::distance(names.begin(), it));
				name_map[*it] = index;
			}
			for (const std::string &input_col : stored_model_data.input_set_names) {
				std::cout << input_col << std::endl;
				auto entry = name_map.find(input_col);
				if (entry == name_map.end()) {
					throw BinderException("Input table should contain the BY feature columns");
				}
				input_mask.push_back(entry->second);
			}
		} else if (!stored_model_data.exclude_set_names.empty()) {
			for (auto it = names.begin(); it != names.end(); ++it) {
				bool exclude_found = false;
				for (const std::string &exclude_col : stored_model_data.exclude_set_names) {
					if (exclude_col == *it) {
						exclude_found = true;
						break;
					}
				}
				if (exclude_found) {
					continue;
				}
				auto index = static_cast<idx_t>(std::distance(names.begin(), it));
				input_mask.push_back(index);
			}
		} else {
			for (size_t i = 0; i < names.size(); i++) {
				input_mask.push_back(i);
			}
		}
		bound_predict->input_mask = std::move(input_mask);

		if (bound_predict->model_type == ModelType::GNN) {
			vector<string> opt_names;
			vector<LogicalType> opt_types;
			result->opt_binder = CreateBinder(context, this);
			result->children.push_back(result->opt_binder->Bind(*ref.opt_source));

			result->opt_binder->bind_context.GetTypesAndNames(opt_names, opt_types);

			vector<idx_t> opt_mask;
			if (!stored_model_data.opt_set_names.empty()) {
				for (const std::string &input_col : stored_model_data.opt_set_names) {
					bool feature_found = false;
					for (auto it = opt_names.begin(); it != opt_names.end(); ++it) {
						auto index = static_cast<idx_t>(std::distance(opt_names.begin(), it));
						if (input_col == *it) {
							opt_mask.push_back(index);
							feature_found = true;
							break;
						}
					}
					if (!feature_found) {
						throw BinderException("Input table should contain the BY feature columns");
					}
				}
			} else if (!stored_model_data.exclude_opt_set_names.empty()) {
				for (auto it = opt_names.begin(); it != opt_names.end(); ++it) {
					bool exclude_found = false;
					for (const std::string &exclude_col : stored_model_data.exclude_opt_set_names) {
						if (exclude_col == *it) {
							exclude_found = true;
							break;
						}
					}
					if (exclude_found) {
						continue;
					}
					auto index = static_cast<idx_t>(std::distance(opt_names.begin(), it));
					opt_mask.push_back(index);
				}
			} else {
				for (size_t i = 0; i < opt_names.size(); i++) {
					opt_mask.push_back(i);
				}
			}
			bound_predict->opt_mask = std::move(opt_mask);
		}
	}

	vector<string> result_names = stored_model_data.out_names;
	names.insert(names.end(), std::make_move_iterator(result_names.begin()),
	             std::make_move_iterator(result_names.end()));

	vector<LogicalType> result_types = stored_model_data.out_types;
	types.insert(types.end(), std::make_move_iterator(result_types.begin()),
	             std::make_move_iterator(result_types.end()));

	if (ref.source) {
		bound_predict->input_set_names = std::move(stored_model_data.input_set_names);
		bound_predict->input_set_types = std::move(input_types);
	}
	bound_predict->types = types;
	bound_predict->result_set_names = std::move(stored_model_data.out_names);
	bound_predict->result_set_types = std::move(stored_model_data.out_types);

	result->bound_predict = std::move(bound_predict);
	auto subquery_alias = ref.alias.empty() ? "__unnamed_predict" : ref.alias;
	bind_context.AddGenericBinding(result->bind_index, subquery_alias, names, types);

	if (ref.source) {
		MoveCorrelatedExpressions(*result->child_binder);
	}
	return std::move(result);
}

unique_ptr<BoundTableRef> Binder::Bind(TablePredictRef &expr) {
	if (expr.source) {
		// Wrap the source in a projection
		auto subquery = make_uniq<SelectNode>();
		subquery->select_list.push_back(make_uniq<StarExpression>());
		subquery->from_table = std::move(expr.source);

		auto subquery_select = make_uniq<SelectStatement>();
		subquery_select->node = std::move(subquery);
		auto subquery_ref = make_uniq<SubqueryRef>(std::move(subquery_select));

		expr.source = std::move(subquery_ref);
	}

	return BindBoundPredict(expr);
}

} // namespace duckdb
