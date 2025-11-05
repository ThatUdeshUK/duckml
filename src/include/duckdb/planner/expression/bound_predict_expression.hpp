//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/planner/expression/bound_function_expression.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/planner/expression.hpp"
#include "duckdb/planner/tableref/bound_predictref.hpp"

namespace duckdb {

//! Represents a function call that has been bound to a base function
class BoundPredictExpression final : public Expression {
public:
	static constexpr auto TYPE = ExpressionClass::PREDICT;

public:
	BoundPredictExpression(LogicalType return_type, vector<unique_ptr<Expression>> arguments);

	//! The bound predict info
	std::unique_ptr<BoundPredictInfo> bound_predict;

	//! List of child-expressions of the function
	vector<unique_ptr<Expression>> children;
public:
	bool IsVolatile() const override;
	bool IsConsistent() const override;
	bool IsFoldable() const override;
	bool CanThrow() const override;

	string ToString() const override;
	hash_t Hash() const override;
	bool Equals(const BaseExpression &other) const override;
	
	bool PropagatesNullValues() const override;
	unique_ptr<Expression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<Expression> Deserialize(Deserializer &deserializer);
};

} // namespace duckdb
