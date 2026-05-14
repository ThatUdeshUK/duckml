#include "duckdb/catalog/catalog_entry/embedding_catalog_entry.hpp"

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/catalog/catalog_entry/schema_catalog_entry.hpp"
#include "duckdb/parser/keyword_helper.hpp"
#include "duckdb/parser/parsed_data/create_embedding_info.hpp"
#include "duckdb/parser/parsed_data/parse_info.hpp"


namespace duckdb {

EmbeddingData::EmbeddingData(CreateEmbeddingInfo &info)
    : table(info.table), column(info.column), model_catalog(info.model_catalog), model_schema(info.model_schema),
      model_name(info.model_name), embedding_size(info.embedding_size) {
}

EmbeddingCatalogEntry::EmbeddingCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateEmbeddingInfo &info)
    : StandardEntry(CatalogType::EMBEDDING_ENTRY, schema, catalog, info.embedding_name), data(info) {
	this->temporary = info.temporary;
	this->comment = info.comment;
	this->tags = info.tags;
}

EmbeddingData EmbeddingCatalogEntry::GetData() const {
	lock_guard<mutex> entry_lock(lock);
	return data;
}

unique_ptr<CatalogEntry> EmbeddingCatalogEntry::Copy(ClientContext &context) const {
	auto info_copy = GetInfo();
	auto &cast_info = info_copy->Cast<CreateEmbeddingInfo>();
	return make_uniq<EmbeddingCatalogEntry>(catalog, schema, cast_info);
}

unique_ptr<CreateInfo> EmbeddingCatalogEntry::GetInfo() const {
	auto d = GetData();

	auto result = make_uniq<CreateEmbeddingInfo>();
	result->catalog = catalog.GetName();
	result->schema = schema.name;
	result->embedding_name = name;
	result->table = d.table;
	result->column = d.column;
	result->model_catalog = d.model_catalog;
	result->model_schema = d.model_schema;
	result->model_name = d.model_name;
	result->embedding_size = d.embedding_size;
	result->dependencies = dependencies;
	result->comment = comment;
	result->tags = tags;
	return std::move(result);
}

string EmbeddingCatalogEntry::ToSQL() const {
	auto d = GetData();

	string result;
	result += "CREATE EMBEDDING ";
	result += KeywordHelper::WriteOptionallyQuoted(name);
	result += " ON ";
	result += KeywordHelper::WriteOptionallyQuoted(d.table);
	result += " (";
	result += KeywordHelper::WriteOptionallyQuoted(d.column);
	result += ") USING ";
	result += ParseInfo::QualifierToString(d.model_catalog, d.model_schema, d.model_name);
	if (d.embedding_size > 0) {
		result += " SIZE ";
		result += std::to_string(d.embedding_size);
	}
	result += ";";
	return result;
}

} // namespace duckdb