//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/parsed_data/create_embedding_info.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/parser/parsed_data/create_info.hpp"

namespace duckdb {

struct CreateEmbeddingInfo : public CreateInfo {
	CreateEmbeddingInfo();

	//! Name of the embedding index
	string embedding_name;
	//! Table the embedding index is built on
	string table;
	//! Column to embed
	string column;
	//! Embedding model (catalog.schema.name)
	string model_catalog;
	string model_schema;
	string model_name;
	//! Vector dimensionality (0 = not specified, inferred from model)
	int embedding_size;

public:
	unique_ptr<CreateInfo> Copy() const override;
	string ToString() const override;

	DUCKDB_API void Serialize(Serializer &serializer) const override;
	DUCKDB_API static unique_ptr<CreateInfo> Deserialize(Deserializer &deserializer);
};

} // namespace duckdb