#include "duckdb/execution/operator/schema/physical_create_embedding.hpp"

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/common/types/value.hpp"
#include "duckdb/parser/column_definition.hpp"
#include "duckdb/parser/parsed_data/alter_table_info.hpp"

namespace duckdb {

SourceResultType PhysicalCreateEmbedding::GetData(ExecutionContext &context, DataChunk &chunk,
                                                  OperatorSourceInput &input) const {
	auto &catalog = Catalog::GetCatalog(context.client, info->catalog);

	// Determine the vector column type: fixed ARRAY when size is known, LIST otherwise.
	LogicalType embedding_type = info->embedding_size > 0
	                                 ? LogicalType::ARRAY(LogicalType::FLOAT, info->embedding_size)
	                                 : LogicalType::LIST(LogicalType::FLOAT);

	// Add the embedding vector column to the table via ALTER TABLE ADD COLUMN.
	AlterEntryData entry_data(info->catalog, info->schema, info->table, OnEntryNotFound::THROW_EXCEPTION);
	ColumnDefinition col_def(info->embedding_name, std::move(embedding_type));
	auto add_col =
	    make_uniq<AddColumnInfo>(std::move(entry_data), std::move(col_def), /*if_column_not_exists=*/true);

	catalog.Alter(context.client, *add_col);

	// Register the catalog entry that records table.column → embedding_name.
	catalog.CreateEmbedding(context.client, *info);

	return SourceResultType::FINISHED;
}

} // namespace duckdb