#include "duckdb/parser/expression/predict_expression.hpp"

#include <utility>
#include "duckdb/common/string_util.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/types/hash.hpp"

#include "duckdb/common/serializer/serializer.hpp"
#include "duckdb/common/serializer/deserializer.hpp"

namespace duckdb {

PredictExpression::PredictExpression() : ParsedExpression(ExpressionType::PREDICT, ExpressionClass::PREDICT) {
}

PredictExpression::PredictExpression(const string &model_name, const string &prompt, vector<unique_ptr<ParsedExpression>> children_p)
    : ParsedExpression(ExpressionType::PREDICT, ExpressionClass::PREDICT), model_name(model_name), 
	  prompt(prompt), children(std::move(children_p)) {
}

string PredictExpression::ToString() const {
	return out_col_name;
}

bool PredictExpression::Equal(const PredictExpression &a, const PredictExpression &b) {
	if (b.children.size() != a.children.size()) {
		return false;
	}
	for (idx_t i = 0; i < a.children.size(); i++) {
		if (!a.children[i]->Equals(*b.children[i])) {
			return false;
		}
	}
	if (a.model_name != b.model_name || a.prompt != b.prompt || a.agg != b.agg) {
		return false;
	}
	return true;
}

hash_t PredictExpression::Hash() const {
	hash_t result = ParsedExpression::Hash();
	result = CombineHash(result, duckdb::Hash<const char *>(model_name.c_str()));
	result = CombineHash(result, duckdb::Hash<const char *>(prompt.c_str()));
	result = CombineHash(result, duckdb::Hash<bool>(agg));
	return result;
}

unique_ptr<ParsedExpression> PredictExpression::Copy() const {
	vector<unique_ptr<ParsedExpression>> copy_children;
	copy_children.reserve(children.size());
	for (auto &child : children) {
		copy_children.push_back(child->Copy());
	}
	auto copy = make_uniq<PredictExpression>(model_name, prompt, std::move(copy_children));
	copy->agg = agg;
	copy->CopyProperties(*this);
	return std::move(copy);
}

void PredictExpression::Verify() const {
	D_ASSERT(!model_name.empty());
}

} // namespace duckdb
