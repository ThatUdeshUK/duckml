#include "duckdb/parser/parsed_data/create_embedding_info.hpp"
#include "duckdb/parser/statement/create_statement.hpp"
#include "duckdb/parser/transformer.hpp"

namespace duckdb {

unique_ptr<CreateStatement> Transformer::TransformCreateEmbedding(duckdb_libpgquery::PGCreateEmbeddingStmt &stmt) {
	auto result = make_uniq<CreateStatement>();
	auto info = make_uniq<CreateEmbeddingInfo>();

	info->embedding_name = stmt.embeddingName;

	auto relation = TransformQualifiedName(*stmt.relation);
	info->catalog = relation.catalog;
	info->schema = relation.schema;
	info->table = relation.name;

	info->column = stmt.column;

	auto model = TransformQualifiedName(*stmt.embeddingModel);
	info->model_catalog = model.catalog;
	info->model_schema = model.schema;
	info->model_name = model.name;

	info->embedding_size = stmt.embeddingSize;
	info->on_conflict = TransformOnConflict(stmt.onconflict);

	result->info = std::move(info);
	return result;
}

} // namespace duckdb