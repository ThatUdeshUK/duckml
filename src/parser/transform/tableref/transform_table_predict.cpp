#include "duckdb/parser/tableref/table_predict_ref.hpp"
#include "duckdb/parser/transformer.hpp"

#include <regex>

namespace duckdb {

unique_ptr<TableRef> Transformer::TransformTablePredict(duckdb_libpgquery::PGPredictExpr &root) {
	auto result = make_uniq<TablePredictRef>();

	auto qname = TransformQualifiedName(*root.model_name);
	result->model_name = qname.name;

	if (root.prompt != nullptr) {
		result->prompt = root.prompt;

		const std::regex in_re(R"(\{\{[A-Za-z_][A-Za-z0-9_]*\}\})");
		auto words_begin = std::sregex_iterator(result->prompt.begin(), result->prompt.end(), in_re);
		auto words_end = std::sregex_iterator();

		for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
			std::smatch match = *i;
			std::string match_str = match.str();
			auto attr = match_str.substr(2, match_str.size() - 4);		
			
			auto *cr = Transformer::MakePGColumnRef(attr.c_str());

			auto target = PGPointerCast<duckdb_libpgquery::PGNode>(cr);
			auto expr = TransformExpression(*target);
			result->parsed_input_columns.push_back(std::move(expr));			
		}
	}

	if (root.embedding) {
		result->is_embedding = true;

		auto *cr = Transformer::MakePGColumnRef(root.input_col);
		auto target = PGPointerCast<duckdb_libpgquery::PGNode>(cr);
		auto expr = TransformExpression(*target);
		result->parsed_input_columns.push_back(std::move(expr));
	}

	if (root.source) {
		result->source = TransformTableRefNode(*root.source);
	}
	
	if (root.has_opt) {
		result->opt_source = TransformTableRefNode(*root.opt_source);
	}
	result->alias = TransformAlias(root.alias, result->column_name_alias);

	SetQueryLocation(*result, root.location);
	return std::move(result);
}

} // namespace duckdb
