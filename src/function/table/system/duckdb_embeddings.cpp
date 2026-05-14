#include "duckdb/function/table/system_functions.hpp"

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/catalog/catalog_entry/embedding_catalog_entry.hpp"
#include "duckdb/catalog/catalog_entry/schema_catalog_entry.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/numeric_utils.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/main/client_data.hpp"

namespace duckdb {

struct DuckDBEmbeddingsData : public GlobalTableFunctionState {
	DuckDBEmbeddingsData() : offset(0) {
	}

	vector<reference<EmbeddingCatalogEntry>> entries;
	idx_t offset;
};

static unique_ptr<FunctionData> DuckDBEmbeddingsBind(ClientContext &context, TableFunctionBindInput &input,
                                                     vector<LogicalType> &return_types, vector<string> &names) {
	names.emplace_back("database_name");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("database_oid");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("schema_name");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("schema_oid");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("embedding_name");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("embedding_oid");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("comment");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("tags");
	return_types.emplace_back(LogicalType::MAP(LogicalType::VARCHAR, LogicalType::VARCHAR));

	names.emplace_back("temporary");
	return_types.emplace_back(LogicalType::BOOLEAN);

	names.emplace_back("table_name");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("column_name");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("model_catalog");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("model_schema");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("model_name");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("embedding_size");
	return_types.emplace_back(LogicalType::INTEGER);

	names.emplace_back("sql");
	return_types.emplace_back(LogicalType::VARCHAR);

	return nullptr;
}

unique_ptr<GlobalTableFunctionState> DuckDBEmbeddingsInit(ClientContext &context, TableFunctionInitInput &input) {
	auto result = make_uniq<DuckDBEmbeddingsData>();

	auto schemas = Catalog::GetAllSchemas(context);
	for (auto &schema : schemas) {
		schema.get().Scan(context, CatalogType::EMBEDDING_ENTRY, [&](CatalogEntry &entry) {
			result->entries.push_back(entry.Cast<EmbeddingCatalogEntry>());
		});
	}
	return std::move(result);
}

void DuckDBEmbeddingsFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
	auto &data = data_p.global_state->Cast<DuckDBEmbeddingsData>();
	if (data.offset >= data.entries.size()) {
		return;
	}

	idx_t count = 0;
	while (data.offset < data.entries.size() && count < STANDARD_VECTOR_SIZE) {
		auto &entry = data.entries[data.offset++].get();
		auto d = entry.GetData();

		idx_t col = 0;
		// database_name, VARCHAR
		output.SetValue(col++, count, Value(entry.catalog.GetName()));
		// database_oid, BIGINT
		output.SetValue(col++, count, Value::BIGINT(NumericCast<int64_t>(entry.catalog.GetOid())));
		// schema_name, VARCHAR
		output.SetValue(col++, count, Value(entry.schema.name));
		// schema_oid, BIGINT
		output.SetValue(col++, count, Value::BIGINT(NumericCast<int64_t>(entry.schema.oid)));
		// embedding_name, VARCHAR
		output.SetValue(col++, count, Value(entry.name));
		// embedding_oid, BIGINT
		output.SetValue(col++, count, Value::BIGINT(NumericCast<int64_t>(entry.oid)));
		// comment, VARCHAR
		output.SetValue(col++, count, Value(entry.comment));
		// tags, MAP(VARCHAR, VARCHAR)
		output.SetValue(col++, count, Value::MAP(entry.tags));
		// temporary, BOOLEAN
		output.SetValue(col++, count, Value::BOOLEAN(entry.temporary));
		// table_name, VARCHAR
		output.SetValue(col++, count, Value(d.table));
		// column_name, VARCHAR
		output.SetValue(col++, count, Value(d.column));
		// model_catalog, VARCHAR
		output.SetValue(col++, count, d.model_catalog.empty() ? Value(LogicalType::VARCHAR) : Value(d.model_catalog));
		// model_schema, VARCHAR
		output.SetValue(col++, count, d.model_schema.empty() ? Value(LogicalType::VARCHAR) : Value(d.model_schema));
		// model_name, VARCHAR
		output.SetValue(col++, count, Value(d.model_name));
		// embedding_size, INTEGER (0 = dynamic / not specified)
		output.SetValue(col++, count, Value::INTEGER(d.embedding_size));
		// sql, VARCHAR
		output.SetValue(col++, count, Value(entry.ToSQL()));

		count++;
	}
	output.SetCardinality(count);
}

void DuckDBEmbeddingsFun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(
	    TableFunction("ipdb_embeddings", {}, DuckDBEmbeddingsFunction, DuckDBEmbeddingsBind, DuckDBEmbeddingsInit));
}

} // namespace duckdb
