#include "duckdb/parser/statement/create_statement.hpp"
#include "duckdb/parser/parsed_data/create_model_info.hpp"
#include "duckdb/parser/transformer.hpp"
#include "duckdb/common/enum_class_hash.hpp"
#include "duckdb/common/unordered_set.hpp"
#include "duckdb/common/operator/cast_operators.hpp"
#include "duckdb/parser/expression/constant_expression.hpp"

namespace duckdb {

class ModelOnInfo {
public:
	QualifiedName rname;
	vector<string> input_set_names;
	vector<string> exclude_set_names;

	QualifiedName oname;
	vector<string> opt_set_names;
	vector<string> exclude_opt_set_names;

	vector<string> out_names;
	vector<LogicalType> out_types;
	case_insensitive_map_t<Value> options;

	bool on_prompt;
	string base_api;
	string secret;
};

unique_ptr<ModelOnInfo> Transformer::TransformModelOn(duckdb_libpgquery::PGModelOn &stmt) {
	auto n = make_uniq<ModelOnInfo>();

	if (stmt.options) {
		for (auto c = stmt.options->head; c != nullptr; c = lnext(c)) {
			auto nadef = PGPointerCast<duckdb_libpgquery::PGNamedArgExpr>(c->data.ptr_value);
			auto naentry = TransformNamedArg(*nadef)->Cast<ConstantExpression>();

			n->options.insert({std::move(naentry.alias), std::move(naentry.value)});
		}
	}

	if (stmt.on_prompt) {
		n->on_prompt = stmt.on_prompt;
		if (stmt.base_api)
			n->base_api = stmt.base_api;
		if (stmt.secret)
			n->secret = TransformQualifiedName(*stmt.secret).name;
		return n;
	}

	if (stmt.rel_name) {
		n->rname = TransformQualifiedName(*stmt.rel_name);
	}

	auto feat_col_node = PGPointerCast<duckdb_libpgquery::PGPredictFeatExpr>(stmt.rel_feat);
	if (feat_col_node && feat_col_node->input_set) {
		for (auto c = feat_col_node->input_set->head; c != nullptr; c = lnext(c)) {
			n->input_set_names.push_back(reinterpret_cast<duckdb_libpgquery::PGValue *>(c->data.ptr_value)->val.str);
		}
	}
	if (feat_col_node && feat_col_node->exclude_set) {
		for (auto c = feat_col_node->exclude_set->head; c != nullptr; c = lnext(c)) {
			n->exclude_set_names.push_back(reinterpret_cast<duckdb_libpgquery::PGValue *>(c->data.ptr_value)->val.str);
		}
	}

	if (stmt.opt_name) {
		n->rname = TransformQualifiedName(*stmt.opt_name);
	}

	auto opt_col_node = PGPointerCast<duckdb_libpgquery::PGPredictFeatExpr>(stmt.opt_feat);
	if (opt_col_node && opt_col_node->input_set) {
		for (auto c = opt_col_node->input_set->head; c != nullptr; c = lnext(c)) {
			n->opt_set_names.push_back(reinterpret_cast<duckdb_libpgquery::PGValue *>(c->data.ptr_value)->val.str);
		}
	}
	if (opt_col_node && opt_col_node->exclude_set) {
		for (auto c = opt_col_node->exclude_set->head; c != nullptr; c = lnext(c)) {
			n->exclude_opt_set_names.push_back(
			    reinterpret_cast<duckdb_libpgquery::PGValue *>(c->data.ptr_value)->val.str);
		}
	}

	for (auto c = stmt.result_set->head; c != nullptr; c = lnext(c)) {
		auto cdef = PGPointerCast<duckdb_libpgquery::PGColumnDef>(c->data.ptr_value);
		auto centry = TransformColumnDefinition(*cdef);
		if (cdef->constraints) {
			throw ParserException("Result set must not contain constraints!");
		}
		n->out_names.push_back(centry.GetName());
		n->out_types.push_back(centry.GetType());
	}

	return n;
}

unique_ptr<CreateStatement> Transformer::TransformCreateModel(duckdb_libpgquery::PGCreateModelStmt &stmt) {
	auto result = make_uniq<CreateStatement>();
	auto info = make_uniq<CreateModelInfo>();

	auto qname = TransformQualifiedName(*stmt.model);
	info->catalog = qname.catalog;
	info->schema = qname.schema;
	info->name = qname.name;

	info->model_type = static_cast<ModelType>(stmt.model_type);
	info->model_path = stmt.model_path;

	auto model_on = TransformModelOn(PGCast<duckdb_libpgquery::PGModelOn>(*stmt.model_on));
	if (model_on->on_prompt) {
		info->on_prompt = true;
		info->base_api = std::move(model_on->base_api);
		info->secret = std::move(model_on->secret);
	} else {
		info->rel_name = model_on->rname.name;
		info->input_set_names = std::move(model_on->input_set_names);
		info->exclude_set_names = std::move(model_on->exclude_set_names);
		info->opt_rel_name = model_on->oname.name;
		info->opt_set_names = std::move(model_on->opt_set_names);
		info->exclude_opt_set_names = std::move(model_on->exclude_opt_set_names);

		info->out_names = std::move(model_on->out_names);
		info->out_types = std::move(model_on->out_types);
		info->on_prompt = false;
	}
	info->options = std::move(model_on->options);

	info->temporary = !stmt.model->relpersistence;
	info->on_conflict = TransformOnConflict(stmt.onconflict);

	result->info = std::move(info);
	return result;
}

} // namespace duckdb
