#include "duckdb/common/enum_util.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/common/prompt.hpp"
#include "duckdb/parser/expression/predict_expression.hpp"
#include "duckdb/parser/transformer.hpp"

#include <regex>
#include <iostream>

namespace duckdb {

duckdb_libpgquery::PGColumnRef* Transformer::MakePGColumnRef(const std::string &col_name) {
    // Allocate the node
    auto *colref = (duckdb_libpgquery::PGColumnRef *) duckdb_libpgquery::newNode(sizeof(duckdb_libpgquery::PGColumnRef), duckdb_libpgquery::T_PGColumnRef);

	size_t ref = col_name.find('.');
    if (ref != std::string::npos) {
		auto table_name = col_name.substr(0, ref);
		auto column_name = col_name.substr(ref + 1);

		std::cout << table_name << "." << column_name << std::endl;

        auto *table = duckdb_libpgquery::makeString(strdup(table_name.c_str()));
        auto *col = duckdb_libpgquery::makeString(strdup(column_name.c_str()));

		auto temp = duckdb_libpgquery::lcons(col, (duckdb_libpgquery::PGList *) NULL);
		colref->fields = duckdb_libpgquery::lcons(table, temp);
    } else {
		auto *pgval = duckdb_libpgquery::makeString(strdup(col_name.c_str()));
		colref->fields = duckdb_libpgquery::lcons(pgval, (duckdb_libpgquery::PGList *) NULL);  // one-element list for column name
    }

    colref->location = -1;               // usually -1 if no SQL text position

    return colref;
}

unique_ptr<ParsedExpression> Transformer::TransformPredict(duckdb_libpgquery::PGPredictExpr &root) {
	auto result = make_uniq<PredictExpression>();

	auto qname = TransformQualifiedName(*root.model_name);
	result->model_name = qname.name;

	if (root.prompt != nullptr)
		result->prompt = root.prompt;

	result->agg = root.agg;

	static const std::regex out_re(Prompt::OUT_REGEX, std::regex_constants::icase);
	auto words_begin = std::sregex_iterator(result->prompt.begin(), result->prompt.end(), out_re);
	auto words_end = std::sregex_iterator();

	idx_t n_cols = 0;
	for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
		if (n_cols > 0)
			throw ParserException("Scalar predict clause only support one output column. At model \"%s\" with \"%s\"", result->model_name.c_str(), result->prompt.c_str());

		std::smatch match = *i;
		result->out_col_name = match[1];
		
		std::string type = match[2].str();
		result->out_col_type = Prompt::type_to_logical_type(type);
		n_cols++;
	}

	static const std::regex in_re(R"(\{\{([A-Za-z_][A-Za-z0-9_]*)(\.[A-Za-z_][A-Za-z0-9_]*)?\}\})");
	words_begin = std::sregex_iterator(result->prompt.begin(), result->prompt.end(), in_re);
	words_end = std::sregex_iterator();

	for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
		std::smatch match = *i;
		std::string match_str = match.str();
		auto attr = match_str.substr(2, match_str.size() - 4);		
		
		result->input_col_names.push_back(attr);

		auto *cr = MakePGColumnRef(attr.c_str());

		auto target = PGPointerCast<duckdb_libpgquery::PGNode>(cr);
		auto expr = TransformExpression(*target);
		result->children.push_back(std::move(expr));			
	}

	SetQueryLocation(*result, root.location);
	return std::move(result);
}

} // namespace duckdb
