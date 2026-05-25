//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/common/column_fill_callback.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/types/vector.hpp"
#include "duckdb/common/typedefs.hpp"

namespace duckdb {

// Abstract callback invoked during DataTable::AddColumn to fill a new column's data.
// Fill is called once per batch in sequential physical-row order; pos advances by count each call.
struct ColumnFillCallback {
	virtual void Fill(Vector &result, idx_t count) = 0;
	virtual ~ColumnFillCallback() = default;
};

} // namespace duckdb
