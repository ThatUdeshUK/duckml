//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/expression/function_expression.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/parser/parsed_expression.hpp"
#include "duckdb/parser/tableref.hpp"

namespace duckdb {
//! Represents a function call
class PredictExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::PREDICT;

public:
	DUCKDB_API PredictExpression();
	DUCKDB_API PredictExpression(const string &model_name, const string &prompt, 
								 vector<unique_ptr<ParsedExpression>> children);

	//! List of arguments to the function
	vector<unique_ptr<ParsedExpression>> children;
	//! The model name for the predict
	string model_name;
	//! The prompt for llm tasks
	string prompt;
	//! Is sem agg
	bool agg;
	//! input column names
	vector<string> input_col_names;
	//! output column names
	string out_col_name;
	//! output column types
	LogicalType out_col_type;

public:
	string ToString() const override;

	unique_ptr<ParsedExpression> Copy() const override;

	static bool Equal(const PredictExpression &a, const PredictExpression &b);
	hash_t Hash() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);

	void Verify() const override;
};
} // namespace duckdb
