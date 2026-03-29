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
	if (source && !source->Equals(*other.source)) {
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
	if (source) {
		copy->source = source->Copy();
	}
	if (opt_source) {
		copy->opt_source = opt_source->Copy();
	}
	copy->model_name = model_name;
	copy->prompt = prompt;
	copy->is_embedding = is_embedding;
	copy->parsed_input_columns.reserve(parsed_input_columns.size());
	for (auto &child : parsed_input_columns) {
		copy->parsed_input_columns.push_back(child->Copy());
	}
	copy->column_name_alias = column_name_alias;
	copy->alias = alias;
	return std::move(copy);
}

} // namespace duckdb
