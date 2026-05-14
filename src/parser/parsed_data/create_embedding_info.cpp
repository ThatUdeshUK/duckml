#include "duckdb/parser/parsed_data/create_embedding_info.hpp"
#include "duckdb/parser/keyword_helper.hpp"

namespace duckdb {

CreateEmbeddingInfo::CreateEmbeddingInfo()
    : CreateInfo(CatalogType::EMBEDDING_ENTRY, INVALID_SCHEMA), embedding_name(string()), table(string()),
      column(string()), model_catalog(string()), model_schema(string()), model_name(string()), embedding_size(0) {
}

unique_ptr<CreateInfo> CreateEmbeddingInfo::Copy() const {
	auto result = make_uniq<CreateEmbeddingInfo>();
	CopyProperties(*result);
	result->embedding_name = embedding_name;
	result->table = table;
	result->column = column;
	result->model_catalog = model_catalog;
	result->model_schema = model_schema;
	result->model_name = model_name;
	result->embedding_size = embedding_size;
	return std::move(result);
}

string CreateEmbeddingInfo::ToString() const {
	string result;
	result += "CREATE EMBEDDING ";
	if (on_conflict == OnCreateConflict::IGNORE_ON_CONFLICT) {
		result += "IF NOT EXISTS ";
	}
	result += KeywordHelper::WriteOptionallyQuoted(embedding_name);
	result += " ON ";
	result += QualifierToString(temporary ? "" : catalog, schema, table);
	result += " (";
	result += KeywordHelper::WriteOptionallyQuoted(column);
	result += ") USING ";
	result += QualifierToString(model_catalog, model_schema, model_name);
	if (embedding_size > 0) {
		result += " SIZE ";
		result += std::to_string(embedding_size);
	}
	result += ";";
	return result;
}

} // namespace duckdb