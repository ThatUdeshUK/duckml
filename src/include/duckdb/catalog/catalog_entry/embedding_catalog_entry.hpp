//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/catalog/catalog_entry/embedding_catalog_entry.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/catalog/standard_entry.hpp"
#include "duckdb/common/mutex.hpp"
#include "duckdb/parser/parsed_data/create_embedding_info.hpp"

namespace duckdb {

struct EmbeddingData {
	explicit EmbeddingData(CreateEmbeddingInfo &info);

	//! Table the embedding index is built on
	string table;
	//! Source column being embedded
	string column;
	//! Embedding model (catalog.schema.name)
	string model_catalog;
	string model_schema;
	string model_name;
	//! Vector dimensionality (0 = not specified / dynamic LIST type)
	int embedding_size;
};

//! An embedding index catalog entry — tracks the table.column → embedding column relationship
class EmbeddingCatalogEntry : public StandardEntry {
public:
	static constexpr const CatalogType Type = CatalogType::EMBEDDING_ENTRY;
	static constexpr const char *Name = "embedding";

public:
	EmbeddingCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateEmbeddingInfo &info);

public:
	unique_ptr<CatalogEntry> Copy(ClientContext &context) const override;
	unique_ptr<CreateInfo> GetInfo() const override;
	string ToSQL() const override;

	EmbeddingData GetData() const;

private:
	mutable mutex lock;
	EmbeddingData data;
};

} // namespace duckdb