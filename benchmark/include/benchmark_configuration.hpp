//===----------------------------------------------------------------------===//
//
//                         DuckDB
//
// benchmark_configuration.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/helper.hpp"
#include "duckdb/common/optional_idx.hpp"

namespace duckdb {

enum class BenchmarkMetaType { NONE, INFO, QUERY };
enum class BenchmarkProfileInfo { NONE, NORMAL, DETAILED };

struct BenchmarkConfiguration {
public:
	constexpr static size_t DEFAULT_TIMEOUT = 30;

public:
	string name_pattern {};
	BenchmarkMetaType meta = BenchmarkMetaType::NONE;
	BenchmarkProfileInfo profile_info = BenchmarkProfileInfo::NONE;
	bool calc_acc = false;
	bool cold_run = true;
	optional_idx timeout_duration = optional_idx(DEFAULT_TIMEOUT);
};

} // namespace duckdb
