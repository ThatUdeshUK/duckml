#pragma once

#include "duckdb/common/types/vector.hpp"
#include "duckdb/common/prompt.hpp"

#include "nlohmann/json.hpp"  // nlohmann/json
#include <regex>
#include <iostream>
#include <sstream>

namespace duckdb {

class PromptUtil {
public:
    std::string extract_json(const std::string &text) {
        size_t start = text.find_first_of("{[");
        if (start == std::string::npos) {
            throw std::runtime_error("No JSON start found");
        }

        char open = text[start];
        char close = (open == '{') ? '}' : ']';

        int depth = 0;
        for (size_t i = start; i < text.size(); i++) {
            if (text[i] == open) {
                depth++;
            } else if (text[i] == close) {
                depth--;
                if (depth == 0) {
                    return text.substr(start, i - start + 1);
                }
            }
        }

        throw std::runtime_error("No matching JSON end found");
    }

    Value extract_longest_integer(const std::string& input) {
        std::regex re("\\d+");
        std::sregex_iterator it(input.begin(), input.end(), re);
        std::sregex_iterator end;

        std::string longest;
        for (; it != end; ++it) {
            if (it->str().size() > longest.size()) {
                longest = it->str();
            }
        }
        if (!longest.empty())
            return Value(std::stoi(longest));
        return Value(LogicalTypeId::INTEGER);
    }

    double extract_double(const nlohmann::json& value) {
        if (value.is_number_float() || value.is_number_integer()) {
            return value.get<double>();
        } else if (value.is_string()) {
            try {
                return std::stod(value.get<std::string>());
            } catch (...) {
                throw std::runtime_error("Invalid number string");
            }
        }
        throw std::runtime_error("Value is not a number or string");
    }

    std::string strip_code_fences(const std::string& input) {
        std::istringstream ss(input);
        std::string line;
        std::ostringstream out;
        while (std::getline(ss, line)) {
            // skip opening and closing ``` lines
            if (line.rfind("```", 0) == 0) continue;
            out << line << "\n";
        }
        return out.str();
    }

    void process_prompt_and_extract_types(std::vector<std::pair<std::string, LogicalTypeId>> &attrs, std::string &prompt) {
		static const std::regex out_re(Prompt::OUT_REGEX, std::regex_constants::icase);
        auto words_begin = std::sregex_iterator(prompt.begin(), prompt.end(), out_re);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            std::string match_str = match.str();
            auto attr = match[1].str();
			auto type = match[2].str();
            auto pos = prompt.find(match_str);
            attrs.push_back(std::make_pair(attr, Prompt::type_to_logical_type(type)));
        }
    }

    std::string embed_prompt(const std::string &prompt, int row, DataChunk &input, const PredictInfo &info, bool is_multi = false) {
        std::string line_end = is_multi ? "`, " : "`;\n";
        std::stringstream ss;
        idx_t col_i = 0;
        for (auto mask_i : info.input_mask) {
            ss << info.input_set_names[col_i] << " = `";
            ss <<  input.GetValue(mask_i, row).ToSQLString() << line_end;
            col_i++;
        }
        return ss.str();
    }

    void extract_array_data(const std::string &llm_out, DataChunk &output, int i, const PredictInfo &info) {
        auto out_json = nlohmann::json::parse(extract_json(llm_out));
        if (out_json.is_array()) {
            output.SetCardinality(output.size() + out_json.size());
            idx_t row = i;
            for (nlohmann::json::iterator it = out_json.begin(); it != out_json.end(); ++it) {
                populate_row_data(*it, row, output, info);
                row++;
            }
        } else {
            std::cout << "JSON parse issue: Array not found" << std::endl;
        }
    }

    void extract_row_data(const std::string &llm_out, int row, DataChunk &output, const PredictInfo &info) {
        try {
            auto json_str = extract_json(llm_out);
            auto out_json = nlohmann::json::parse(json_str);
            populate_row_data(out_json, row, output, info);
        } catch (const nlohmann::json::parse_error& e) {
            std::cout << "JSON parse issue: " << e.what() << std::endl;
            fill_null(row, output, info);
        }
    }

    void populate_row_data(const nlohmann::json &out_json, int row, DataChunk &output, const PredictInfo &info) {
        for (size_t j = 0; j < info.result_set_names.size(); j++) {
            auto output_type = info.result_set_types[j];
            auto col_name = info.result_set_names[j];

            try {
                if (!out_json.contains(col_name)) {
                    output.SetValue(j, row, Value(output_type));
                    continue;
                }

                if (output_type == LogicalTypeId::VARCHAR && out_json[col_name].is_string()) {
                    std::string value = out_json[col_name].get<std::string>();
                    output.SetValue(j, row, Value(value));
                } else if (output_type == LogicalTypeId::INTEGER && out_json[col_name].is_string()) {
                    auto value = extract_longest_integer(out_json[col_name].get<std::string>());
                    output.SetValue(j, row, value);
                } else if (output_type == LogicalTypeId::INTEGER && out_json[col_name].is_number()) {
                    int value = out_json[col_name].get<int>();
                    output.SetValue(j, row, Value(value));
                } else if (output_type == LogicalTypeId::DOUBLE) {
                    double value = extract_double(out_json[col_name]);
                    output.SetValue(j, row, Value(value));
                } else if (output_type == LogicalTypeId::BOOLEAN && out_json[col_name].is_boolean()) {
                    bool value = out_json[col_name].get<bool>();
                    output.SetValue(j, row, Value(value));
                }  else {
                    output.SetValue(j, row, Value(output_type));
                }
            } catch (const nlohmann::json::parse_error& e) {
                std::cout << "JSON parse issue: " << e.what() << std::endl;
                output.SetValue(j, row, Value(output_type));
            } catch (const std::runtime_error& e) {
                std::cout << "Conversion error: " << e.what() << std::endl;
                output.SetValue(j, row, Value(output_type));
            }
        }
    }

    void fill_null(int row, DataChunk &output, const PredictInfo &info) {
        for (size_t j = 0; j < info.result_set_names.size(); j++) {
            auto output_type = info.result_set_types[j];
            auto col_name = info.result_set_names[j];

            output.SetValue(j, row, Value(output_type));
        }
    }
};

}