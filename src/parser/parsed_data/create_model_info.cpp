#include "duckdb/parser/parsed_data/create_model_info.hpp"
#include "duckdb/common/enum_util.hpp"

namespace duckdb {

CreateModelInfo::CreateModelInfo()
    : CreateInfo(CatalogType::MODEL_ENTRY, INVALID_SCHEMA), name(string()), model_type(ModelType::TABULAR),
      model_path(string()), on_prompt(false) {
}

unique_ptr<CreateInfo> CreateModelInfo::Copy() const {
	auto result = make_uniq<CreateModelInfo>();
	CopyProperties(*result);
	result->name = name;
	result->schema = schema;
	result->model_type = model_type;
	result->model_path = model_path;
	result->base_api = base_api;
	result->secret = secret;
	result->rel_name = rel_name;
	result->input_set_names = input_set_names;
	result->exclude_set_names = exclude_set_names;
	result->opt_rel_name = opt_rel_name;
	result->opt_set_names = opt_set_names;
	result->exclude_opt_set_names = exclude_opt_set_names;
	result->out_names = out_names;
	result->out_types = out_types;
	result->options = options;
	return std::move(result);
}

string CreateModelInfo::ToString() const {
	std::stringstream ss;
	ss << "CREATE ";
	if (on_conflict == OnCreateConflict::REPLACE_ON_CONFLICT) {
		ss << "OR REPLACE ";
	}
	if (temporary) {
		ss << "TEMPORARY ";
	}
	ss << EnumUtil::ToString<ModelType>(model_type);
	ss << " MODEL ";
	if (on_conflict == OnCreateConflict::IGNORE_ON_CONFLICT) {
		ss << " IF NOT EXISTS ";
	}
	ss << QualifierToString(temporary ? "" : catalog, schema, name);
	ss << " PATH '" << model_path;
	// TODO: - add model on expression here
	ss << "' OUTPUT(...); ";
	return ss.str();
}

} // namespace duckdb
