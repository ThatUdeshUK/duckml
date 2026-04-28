//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/common/enums/optimizer_type.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/constants.hpp"
#include "duckdb/common/vector.hpp"

namespace duckdb {

enum class ModelSelectStrategy : uint8_t { FIRST, RANDOM, OPTIMAL };

string ModelSelectStrategyToString(ModelSelectStrategy type);
ModelSelectStrategy ModelSelectStrategyFromString(const string &str);
vector<string> ListAllModelSelectStrategies();

} // namespace duckdb
