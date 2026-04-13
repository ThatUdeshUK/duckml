#include "duckdb/optimizer/split_predict_join_to_extract_predicate.hpp"

#include "duckdb/optimizer/column_binding_replacer.hpp"
#include "duckdb/optimizer/optimizer.hpp"
#include "duckdb/planner/binder.hpp"
#include "duckdb/planner/column_binding_map.hpp"
#include "duckdb/planner/expression/bound_columnref_expression.hpp"
#include "duckdb/planner/expression/bound_comparison_expression.hpp"
#include "duckdb/planner/expression/bound_predict_expression.hpp"
#include "duckdb/planner/expression_iterator.hpp"
#include "duckdb/planner/operator/logical_filter.hpp"
#include "duckdb/planner/operator/logical_get.hpp"
#include "duckdb/planner/operator/logical_projection.hpp"

#if defined(ENABLE_PREDICT) && defined(ENABLE_LLM_API)
#include "openai.hpp"
#endif

namespace duckdb {

// ---------------------------------------------------------------------------
// Local helper: check whether a join operator type is one we handle.
// ---------------------------------------------------------------------------
static bool IsSupportedJoin(LogicalOperatorType type) {
	return type == LogicalOperatorType::LOGICAL_CROSS_PRODUCT ||
		type == LogicalOperatorType::LOGICAL_JOIN ||
		type == LogicalOperatorType::LOGICAL_COMPARISON_JOIN;
}

// ---------------------------------------------------------------------------
// Local helper: depth-first search for the first BoundPredictExpression in an
// expression tree.  Returns nullptr when none is found.
// ---------------------------------------------------------------------------
static BoundPredictExpression *FindFirstPredict(Expression &expr) {
	if (expr.expression_class == ExpressionClass::PREDICT) {
		return &expr.Cast<BoundPredictExpression>();
	}
	BoundPredictExpression *result = nullptr;
	ExpressionIterator::EnumerateChildren(expr, [&](Expression &child) {
		if (!result) {
			result = FindFirstPredict(child);
		}
	});
	return result;
}

// ---------------------------------------------------------------------------
// Local helper: collect all BoundColumnRefExpression bindings in an expression
// tree into `out`.
// ---------------------------------------------------------------------------
static void CollectColumnBindings(Expression &expr, vector<ColumnBinding> &out) {
	if (expr.expression_class == ExpressionClass::BOUND_COLUMN_REF) {
		out.push_back(expr.Cast<BoundColumnRefExpression>().binding);
		return;
	}
	ExpressionIterator::EnumerateChildren(expr, [&](Expression &child) {
		CollectColumnBindings(child, out);
	});
}

#if defined(ENABLE_PREDICT) && defined(ENABLE_LLM_API)

// ---------------------------------------------------------------------------
// Local helper: recursively collect LogicalGet nodes and build a schema string
// like "CPUs(PRODUCT_ID, PRODUCT_NAME), Motherboards(PRODUCT_ID, PRODUCT_NAME)".
// ---------------------------------------------------------------------------
static void AppendTableSchemas(LogicalOperator &op, std::string &out) {
	if (op.type == LogicalOperatorType::LOGICAL_GET) {
		auto &get = op.Cast<LogicalGet>();
		if (!out.empty()) {
			out += ", ";
		}
		out += get.GetName() + "(";
		for (idx_t i = 0; i < get.names.size(); i++) {
			if (i > 0) {
				out += ", ";
			}
			out += get.names[i];
		}
		out += ")";
	}
	for (auto &child : op.children) {
		AppendTableSchemas(*child, out);
	}
}

// ---------------------------------------------------------------------------
// Holds the structured response from the decomposition LLM call.
// ---------------------------------------------------------------------------
struct DecomposedPredict {
	std::string task1;
	std::string task1_output_column;
	std::string task2;
	std::string task2_output_column;
	std::string predicate;
};

// ---------------------------------------------------------------------------
// Local helper: call the LLM to decompose a cross-join PREDICT into two
// single-input extraction tasks plus a relational predicate.
// ---------------------------------------------------------------------------
static DecomposedPredict CallDecompositionLLM(Optimizer &optimizer,
                                              const BoundPredictInfo &predict_info,
                                              LogicalOperator &join) {
	using Json = nlohmann::json;

	auto &db = DatabaseInstance::GetDatabase(optimizer.context);
	auto api = OpenAI::createInstance(db, predict_info.base_api, predict_info.secret);

	// Build the schema string from the tables under the join.
	std::string schemas;
	AppendTableSchemas(join, schemas);

	// User message: ask the LLM to split the task.
	std::string user_msg =
	    "Convert the following task `" + predict_info.prompt +
	    "` to two separate extraction tasks and a predicate.\n"
	    " Extraction task is a prompt like above that takes a single input and produces an attribute"
	    " that can be used to apply the predicate.\n"
	    " Predicate should be a relational operation.\n"
	    " Schemas: " + schemas;

	// Fixed system message describing the semantic task template.
	std::string sys_msg =
	    "You are a database optimizer with knowledge about semantic tasks.\n"
	    " Semantic task template, `extract the {<output_column> VARCHAR} from {{input_column}}`,"
	    " where <input_column> and <output_column> should be replaced appropriately.";

	// Response schema requiring exactly the five fields.
	Json response_schema;
	response_schema["type"] = "json_schema";
	response_schema["json_schema"] = {
	    {"name", "json_response"},
	    {"strict", true},
	    {"schema", {
	        {"type", "object"},
	        {"additionalProperties", false},
	        {"required", {"task1", "task1_output_column", "task2", "task2_output_column", "predicate"}},
	        {"properties", {
	            {"task1",                {{"type", "string"}}},
	            {"task1_output_column",  {{"type", "string"}}},
	            {"task2",                {{"type", "string"}}},
	            {"task2_output_column",  {{"type", "string"}}},
	            {"predicate",            {{"type", "string"}}}
	        }}
	    }}
	};

	Json request;
	request["model"] = predict_info.model_path;
	request["messages"] = {
	    {{"role", "system"}, {"content", sys_msg}},
	    {{"role", "user"},   {"content", user_msg}}
	};
	request["response_format"] = response_schema;

	auto completion = api->post("chat/completions", request);

	DecomposedPredict result;
	try {
		std::string content = completion["choices"][0]["message"]["content"].get<std::string>();
		auto parsed = Json::parse(PromptUtil::extract_json(content));
		result.task1               = parsed["task1"].get<std::string>();
		result.task1_output_column = parsed["task1_output_column"].get<std::string>();
		result.task2               = parsed["task2"].get<std::string>();
		result.task2_output_column = parsed["task2_output_column"].get<std::string>();
		result.predicate           = parsed["predicate"].get<std::string>();
	} catch (const std::exception &e) {
		throw InternalException("SplitPredictJoin: failed to parse decomposition response: %s", e.what());
	}
	return result;
}

// ---------------------------------------------------------------------------
// Local helper: infer a comparison ExpressionType from the LLM-generated
// predicate string. Checks multi-char tokens before single-char ones.
// ---------------------------------------------------------------------------
static ExpressionType ParseComparisonOp(const std::string &pred) {
	if (pred.find("!=") != std::string::npos || pred.find("<>") != std::string::npos) {
		return ExpressionType::COMPARE_NOTEQUAL;
	}
	if (pred.find("<=") != std::string::npos) {
		return ExpressionType::COMPARE_LESSTHANOREQUALTO;
	}
	if (pred.find(">=") != std::string::npos) {
		return ExpressionType::COMPARE_GREATERTHANOREQUALTO;
	}
	if (pred.find("<") != std::string::npos) {
		return ExpressionType::COMPARE_LESSTHAN;
	}
	if (pred.find(">") != std::string::npos) {
		return ExpressionType::COMPARE_GREATERTHAN;
	}
	return ExpressionType::COMPARE_EQUAL;
}

#endif // ENABLE_PREDICT && ENABLE_LLM_API

// ===========================================================================
// SplitPredictJoinToExtractPredicate
// ===========================================================================

SplitPredictJoinToExtractPredicate::SplitPredictJoinToExtractPredicate(Optimizer &optimizer)
    : optimizer(optimizer) {
}

// ---------------------------------------------------------------------------
bool SplitPredictJoinToExtractPredicate::CanOptimize(LogicalOperator &op) {
	if (op.type != LogicalOperatorType::LOGICAL_FILTER) {
		return false;
	}
	// A limit propagated into the filter means a LIMIT operator sits above;
	// skip the cross-join decomposition in that case.
	if (op.Cast<LogicalFilter>().limit != -1) {
		return false;
	}
	if (op.children.empty()) {
		return false;
	}

	auto &direct_child = *op.children[0];
	if (IsSupportedJoin(direct_child.type)) {
		// FILTER → JOIN
	} else if (direct_child.type == LogicalOperatorType::LOGICAL_PROJECTION &&
	           !direct_child.children.empty() && IsSupportedJoin(direct_child.children[0]->type)) {
		// FILTER → PROJECT → JOIN
	} else {
		return false;
	}

	// Get the output column bindings of the direct child (join or project).
	// When there is a PROJECT between the FILTER and the JOIN, the PREDICT's
	// column refs point to the PROJECT's output — not to the original join
	// table indices — so we must match against the direct child's bindings.
	auto child_output = direct_child.GetColumnBindings();

	auto &filter = op.Cast<LogicalFilter>();
	for (auto &expr : filter.expressions) {
		auto *predict = FindFirstPredict(*expr);
		if (!predict) {
			continue;
		}

		// Collect every BoundColumnRefExpression binding inside the PREDICT.
		vector<ColumnBinding> predict_refs;
		CollectColumnBindings(*predict, predict_refs);

		// Count how many distinct child-output columns the PREDICT references.
		idx_t matched = 0;
		for (auto &cb : child_output) {
			for (auto &ref : predict_refs) {
				if (ref == cb) {
					matched++;
					break;
				}
			}
		}
		if (matched >= 2) {
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
unique_ptr<LogicalOperator>
SplitPredictJoinToExtractPredicate::TransformFilterJoin(unique_ptr<LogicalOperator> op) {
	D_ASSERT(op->type == LogicalOperatorType::LOGICAL_FILTER);

	auto &filter      = op->Cast<LogicalFilter>();
	auto &join        = *op->children[0];

	// Ensure join types are resolved so we can build column-ref expressions.
	if (join.types.empty()) {
		join.ResolveOperatorTypes();
	}

	auto join_bindings = join.GetColumnBindings();
	auto &join_types   = join.types;
	D_ASSERT(join_bindings.size() == join_types.size());

#if defined(ENABLE_PREDICT) && defined(ENABLE_LLM_API)
	// ------------------------------------------------------------------
	// 1. Find the qualifying PREDICT and decompose it via the LLM.
	// ------------------------------------------------------------------
	BoundPredictExpression *orig_predict = nullptr;
	for (auto &expr : filter.expressions) {
		orig_predict = FindFirstPredict(*expr);
		if (orig_predict) {
			break;
		}
	}
	D_ASSERT(orig_predict && orig_predict->bound_predict);
	D_ASSERT(orig_predict->children.size() >= 2);

	auto &orig_info = *orig_predict->bound_predict;
	// Capture strings now — orig_info becomes dangling once the filter
	// expressions are cleared in step 5.
	const std::string orig_prompt = orig_info.prompt;
	const std::string orig_model  = orig_info.model_path;
	auto decomposed = CallDecompositionLLM(optimizer, orig_info, join);

	// ------------------------------------------------------------------
	// 2. Map each PREDICT child back to its position in join_bindings.
	//    children[0] → task1 input, children[1] → task2 input.
	// ------------------------------------------------------------------
	auto find_join_col = [&](const ColumnBinding &cb) -> idx_t {
		for (idx_t i = 0; i < join_bindings.size(); i++) {
			if (join_bindings[i] == cb) {
				return i;
			}
		}
		return DConstants::INVALID_INDEX;
	};

	const idx_t task1_input_col =
	    find_join_col(orig_predict->children[0]->Cast<BoundColumnRefExpression>().binding);
	const idx_t task2_input_col =
	    find_join_col(orig_predict->children[1]->Cast<BoundColumnRefExpression>().binding);
	D_ASSERT(task1_input_col != DConstants::INVALID_INDEX);
	D_ASSERT(task2_input_col != DConstants::INVALID_INDEX);

	// ------------------------------------------------------------------
	// 3. Proj1: pass through all join columns + PREDICT(task1) appended.
	//    Output: {proj1_idx, 0..N-1} = join cols, {proj1_idx, N} = task1
	// ------------------------------------------------------------------
	idx_t proj1_idx = optimizer.binder.GenerateTableIndex();
	{
		vector<unique_ptr<Expression>> proj1_exprs;
		proj1_exprs.reserve(join_bindings.size() + 1);
		for (idx_t i = 0; i < join_bindings.size(); i++) {
			proj1_exprs.push_back(make_uniq<BoundColumnRefExpression>(join_types[i], join_bindings[i]));
		}

		auto task1_info               = make_uniq<BoundPredictInfo>();
		task1_info->model_type         = orig_info.model_type;
		task1_info->model_name         = orig_info.model_name;
		task1_info->model_path         = orig_info.model_path;
		task1_info->base_api           = orig_info.base_api;
		task1_info->secret             = orig_info.secret;
		task1_info->prompt             = decomposed.task1;
		task1_info->result_set_names   = {decomposed.task1_output_column};
		task1_info->result_set_types   = {LogicalType::VARCHAR};
		task1_info->input_set_names    = orig_info.input_set_names.size() > 0
		                                    ? vector<string> {orig_info.input_set_names[0]}
		                                    : vector<string> {"input"};
		task1_info->options            = orig_info.options;

		vector<unique_ptr<Expression>> task1_args;
		task1_args.push_back(make_uniq<BoundColumnRefExpression>(
		    join_types[task1_input_col], join_bindings[task1_input_col]));

		auto task1_predict             = make_uniq<BoundPredictExpression>(LogicalType::VARCHAR,
		                                                                   std::move(task1_args));
		task1_predict->bound_predict   = std::move(task1_info);
		proj1_exprs.push_back(std::move(task1_predict));

		auto proj1 = make_uniq<LogicalProjection>(proj1_idx, std::move(proj1_exprs));
		proj1->children.push_back(std::move(op->children[0]));
		op->children[0] = std::move(proj1);
	}

	// ------------------------------------------------------------------
	// 4. Proj2: pass through all Proj1 columns + PREDICT(task2) appended.
	//    task2's input column is remapped to Proj1 output space.
	//    Output: {proj2_idx, 0..N-1} = join cols,
	//            {proj2_idx, N}   = task1 output (passthrough),
	//            {proj2_idx, N+1} = task2 output
	// ------------------------------------------------------------------
	idx_t proj2_idx = optimizer.binder.GenerateTableIndex();
	const idx_t task1_out_pos = join_bindings.size();      // col N in proj1/proj2
	const idx_t task2_out_pos = join_bindings.size() + 1;  // col N+1 in proj2
	{
		vector<unique_ptr<Expression>> proj2_exprs;
		proj2_exprs.reserve(join_bindings.size() + 2);
		for (idx_t i = 0; i < join_bindings.size(); i++) {
			proj2_exprs.push_back(make_uniq<BoundColumnRefExpression>(
			    join_types[i], ColumnBinding(proj1_idx, i)));
		}
		// task1 output passthrough.
		proj2_exprs.push_back(make_uniq<BoundColumnRefExpression>(
		    LogicalType::VARCHAR, ColumnBinding(proj1_idx, task1_out_pos)));

		auto task2_info               = make_uniq<BoundPredictInfo>();
		task2_info->model_type         = orig_info.model_type;
		task2_info->model_name         = orig_info.model_name;
		task2_info->model_path         = orig_info.model_path;
		task2_info->base_api           = orig_info.base_api;
		task2_info->secret             = orig_info.secret;
		task2_info->prompt             = decomposed.task2;
		task2_info->result_set_names   = {decomposed.task2_output_column};
		task2_info->result_set_types   = {LogicalType::VARCHAR};
		task2_info->input_set_names    = orig_info.input_set_names.size() > 1
		                                    ? vector<string> {orig_info.input_set_names[1]}
		                                    : vector<string> {"input"};
		task2_info->options            = orig_info.options;

		// task2 input is the join column remapped through Proj1.
		vector<unique_ptr<Expression>> task2_args;
		task2_args.push_back(make_uniq<BoundColumnRefExpression>(
		    join_types[task2_input_col], ColumnBinding(proj1_idx, task2_input_col)));

		auto task2_predict             = make_uniq<BoundPredictExpression>(LogicalType::VARCHAR,
		                                                                   std::move(task2_args));
		task2_predict->bound_predict   = std::move(task2_info);
		proj2_exprs.push_back(std::move(task2_predict));

		auto proj2 = make_uniq<LogicalProjection>(proj2_idx, std::move(proj2_exprs));
		proj2->children.push_back(std::move(op->children[0]));
		op->children[0] = std::move(proj2);
	}

	// ------------------------------------------------------------------
	// 5. Rewrite the filter: replace all expressions with a single
	//    comparison between the two extraction outputs from Proj2.
	// ------------------------------------------------------------------
	{
		auto lhs = make_uniq<BoundColumnRefExpression>(
		    LogicalType::VARCHAR, ColumnBinding(proj2_idx, task1_out_pos));
		auto rhs = make_uniq<BoundColumnRefExpression>(
		    LogicalType::VARCHAR, ColumnBinding(proj2_idx, task2_out_pos));
		auto cmp = make_uniq<BoundComparisonExpression>(
		    ParseComparisonOp(decomposed.predicate), std::move(lhs), std::move(rhs));

		filter.expressions.clear();
		filter.expressions.push_back(std::move(cmp));
	}

	// ------------------------------------------------------------------
	// Print the plan transformation for debugging.
	// ------------------------------------------------------------------
	auto op_to_str = [](ExpressionType t) -> const char * {
		switch (t) {
		case ExpressionType::COMPARE_EQUAL:                return "=";
		case ExpressionType::COMPARE_NOTEQUAL:             return "!=";
		case ExpressionType::COMPARE_LESSTHAN:             return "<";
		case ExpressionType::COMPARE_GREATERTHAN:          return ">";
		case ExpressionType::COMPARE_LESSTHANOREQUALTO:    return "<=";
		case ExpressionType::COMPARE_GREATERTHANOREQUALTO: return ">=";
		default:                                           return "?";
		}
	};
	const char *op_sym = op_to_str(ParseComparisonOp(decomposed.predicate));

	std::cout << "\n[SplitPredictJoin] Plan rewrite\n"
	          << "  Original PREDICT prompt : " << orig_prompt << "\n"
	          << "  Model                   : " << orig_model << "\n"
	          << "  Decomposed task1 prompt : " << decomposed.task1 << "\n"
	          << "  Task1 output column     : " << decomposed.task1_output_column << "\n"
	          << "  Decomposed task2 prompt : " << decomposed.task2 << "\n"
	          << "  Task2 output column     : " << decomposed.task2_output_column << "\n"
	          << "  New plan structure:\n"
	          << "    FILTER [" << decomposed.task1_output_column
	          <<              " " << op_sym << " "
	          <<              decomposed.task2_output_column << "]\n"
	          << "      Proj2 [join cols + task1_passthrough + PREDICT(\""
	          <<             decomposed.task2 << "\")]\n"
	          << "        Proj1 [join cols + PREDICT(\""
	          <<               decomposed.task1 << "\")]\n"
	          << "          CROSS_PRODUCT\n\n" << std::endl;

	// ------------------------------------------------------------------
	// 6. Record binding fix so operators above this filter are updated:
	//    old join binding[i] → {proj2_idx, i}
	// ------------------------------------------------------------------
	{
		BindingFix fix;
		fix.filter = op.get();
		fix.old_bindings.reserve(join_bindings.size());
		fix.new_bindings.reserve(join_bindings.size());
		for (idx_t i = 0; i < join_bindings.size(); i++) {
			fix.old_bindings.push_back(join_bindings[i]);
			fix.new_bindings.push_back(ColumnBinding(proj2_idx, i));
		}
		pending_fixes.push_back(std::move(fix));
	}

#endif // ENABLE_PREDICT && ENABLE_LLM_API

	return op;
}

// ---------------------------------------------------------------------------
unique_ptr<LogicalOperator>
SplitPredictJoinToExtractPredicate::Transform(unique_ptr<LogicalOperator> op) {
	if (CanOptimize(*op)) {
		op = TransformFilterJoin(std::move(op));
	}
	for (auto &child : op->children) {
		child = Transform(std::move(child));
	}
	return op;
}

// ---------------------------------------------------------------------------
unique_ptr<LogicalOperator>
SplitPredictJoinToExtractPredicate::Optimize(unique_ptr<LogicalOperator> plan) {
	// Phase 1: find and transform all qualifying filter+join patterns.
	plan = Transform(std::move(plan));

	// Phase 2: for every transformed filter, apply a ColumnBindingReplacer on
	// the operators that sit *above* the filter so that their column refs are
	// updated from the old join bindings to the new Proj2 bindings.
	for (auto &fix : pending_fixes) {
		D_ASSERT(fix.old_bindings.size() == fix.new_bindings.size());

		ColumnBindingReplacer replacer;
		for (idx_t i = 0; i < fix.old_bindings.size(); i++) {
			replacer.replacement_bindings.emplace_back(fix.old_bindings[i], fix.new_bindings[i]);
		}
		// Stop at the filter itself – it and its new children are already correct.
		replacer.stop_operator = fix.filter;
		replacer.VisitOperator(*plan);
	}

	return plan;
}

} // namespace duckdb