#include "duckdb/parser/tableref/table_predict_ref.hpp"

namespace duckdb {

//===--------------------------------------------------------------------===//
// TablePredictRef
//===--------------------------------------------------------------------===//
string TablePredictRef::ToString() const {
	string result = source->ToString();
	result += " PREDICT USING " + model_name;
	return result;
}

bool TablePredictRef::Equals(const TableRef &other_p) const {
	if (!TableRef::Equals(other_p)) {
		return false;
	}
	auto &other = other_p.Cast<TablePredictRef>();
	if (!source->Equals(*other.source)) {
		return false;
	}
	if (model_name != other.model_name) {
		return false;
	}
	if (alias != other.alias) {
		return false;
	}
	return true;
}

unique_ptr<TableRef> TablePredictRef::Copy() {
	auto copy = make_uniq<TablePredictRef>();
	copy->source = source->Copy();
	if (opt_source) {
		copy->opt_source = opt_source->Copy();
	}
	copy->model_name = model_name;
	copy->prompt = prompt;
	copy->alias = alias;
	return std::move(copy);
}

} // namespace duckdb
