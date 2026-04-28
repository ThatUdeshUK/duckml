//===----------------------------------------------------------------------===//
// Optimal model selector — implements the palimpzest get_optimal_models
// algorithm for iPDb.  Self-contained: only depends on the C++ standard library.
//
// Algorithm summary (mirrors palimpzest/utils/model_helpers.py::get_optimal_models):
//   1. Gather MMLU-Pro quality score, USD/output-token cost, and sec/output-token
//      latency for each candidate model from a built-in metrics database.
//   2. Filter candidates that satisfy optional policy constraints.
//   3. If no candidate passes the constraints, fall back to the single best model
//      for the policy's primary metric (highest weight wins).
//   4. Min-max normalise the three metrics across surviving candidates.
//      Cost and latency are inverted so that "higher normalised value = better".
//   5. Score each candidate:  w_q*norm_quality + w_c*norm_cost + w_t*norm_time.
//   6. Return the index of the highest-scoring candidate.
//===----------------------------------------------------------------------===//

#pragma once

#include <algorithm>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

namespace duckdb {

// ── Model metrics ─────────────────────────────────────────────────────────────

struct ModelMetrics {
	double quality; // MMLU-Pro score  (0-100 scale)
	double cost;    // USD per output token
	double time;    // seconds per output token
};

// Static model metrics database derived from palimpzest's pz_models_information.json.
// Keys use the bare model identifier as it appears in CREATE LLM MODEL … PATH '…'.
inline const std::unordered_map<std::string, ModelMetrics> &GetModelMetricsDB() {
	// clang-format off
	static const std::unordered_map<std::string, ModelMetrics> db = {
	    // ── OpenAI ────────────────────────────────────────────────────────────
	    {"gpt-5-2025-08-07",                              {87.00, 1.00e-05, 0.00600}},
	    {"gpt-5",                                         {87.00, 1.00e-05, 0.00600}},
	    {"gpt-5.2-2025-12-11",                           {86.23, 1.40e-05, 0.01471}},
	    {"gpt-5-mini-2025-08-07",                        {82.50, 2.00e-06, 0.01350}},
	    {"gpt-5-mini",                                   {82.50, 2.00e-06, 0.01350}},
	    {"o4-mini-2025-04-16",                           {80.60, 4.40e-06, 0.00920}},
	    {"o4-mini",                                      {80.60, 4.40e-06, 0.00920}},
	    {"gpt-4.1-2025-04-14",                           {80.50, 8.00e-06, 0.00760}},
	    {"gpt-4.1",                                      {80.50, 8.00e-06, 0.00760}},
	    {"gpt-5-nano-2025-08-07",                        {77.90, 4.00e-07, 0.00550}},
	    {"gpt-5-nano",                                   {77.90, 4.00e-07, 0.00550}},
	    {"gpt-4.1-mini-2025-04-14",                      {77.20, 1.60e-06, 0.01610}},
	    {"gpt-4.1-mini",                                 {77.20, 1.60e-06, 0.01610}},
	    {"gpt-4o-2024-08-06",                            {74.10, 1.00e-05, 0.00800}},
	    {"gpt-4o",                                       {74.10, 1.00e-05, 0.00800}},
	    {"gpt-4o-mini-2024-07-18",                       {62.70, 6.00e-07, 0.01590}},
	    {"gpt-4o-mini",                                  {62.70, 6.00e-07, 0.01590}},
	    {"gpt-4.1-nano-2025-04-14",                      {62.30, 4.00e-07, 0.00600}},
	    {"gpt-4.1-nano",                                 {62.30, 4.00e-07, 0.00600}},
	    // ── Anthropic ─────────────────────────────────────────────────────────
	    {"claude-sonnet-4-5-20250929",                   {87.36, 1.50e-05, 0.01272}},
	    {"claude-opus-4-5-20251101",                     {87.30, 2.50e-05, 0.01642}},
	    {"claude-sonnet-4-20250514",                     {83.87, 1.50e-05, 0.01403}},
	    {"claude-3-7-sonnet-20250219",                   {80.70, 1.50e-05, 0.01560}},
	    {"claude-haiku-4-5-20251001",                    {78.72, 1.25e-06, 0.00845}},
	    {"claude-3-5-haiku-20241022",                    {64.10, 4.00e-06, 0.01890}},
	    // ── Google Gemini ──────────────────────────────────────────────────────
	    {"gemini-3-pro-image-preview",                   {90.10, 1.20e-05, 0.00758}},
	    {"gemini-3-pro-preview",                         {90.10, 1.20e-05, 0.00758}},
	    {"gemini-3-flash-preview",                       {87.63, 3.00e-06, 0.00457}},
	    {"gemini-2.5-pro",                               {84.10, 1.00e-05, 0.00720}},
	    {"gemini-2.5-flash",                             {80.75, 2.50e-06, 0.00440}},
	    {"gemini-2.5-flash-lite",                        {79.10, 4.00e-07, 0.00340}},
	    {"gemini-2.0-flash",                             {77.40, 6.00e-07, 0.00540}},
	    // ── Meta / Llama (Together AI) ─────────────────────────────────────────
	    {"meta-llama/Llama-3.3-70B-Instruct-Turbo",     {69.90, 8.80e-07, 0.01220}},
	    {"Llama-3.3-70B-Instruct-Turbo",                {69.90, 8.80e-07, 0.01220}},
	    {"meta-llama/Llama-3.2-90B-Vision-Instruct-Turbo", {65.00, 1.20e-06, 0.03030}},
	    {"meta-llama/Meta-Llama-3.1-8B-Instruct-Turbo", {44.25, 1.80e-07, 0.00500}},
	    {"Meta-Llama-3.1-8B-Instruct-Turbo",            {44.25, 1.80e-07, 0.00500}},
	    {"meta-llama/Llama-3.2-3B-Instruct-Turbo",      {36.50, 6.00e-08, 0.00790}},
	    {"Llama-3.2-3B-Instruct-Turbo",                 {36.50, 6.00e-08, 0.00790}},
	    // ── DeepSeek ──────────────────────────────────────────────────────────
	    {"deepseek-ai/DeepSeek-R1-0528-Qwen3-8B",       {85.00, 9.00e-08, 0.02000}},
	    {"deepseek-ai/DeepSeek-V3",                     {73.80, 1.25e-06, 0.01140}},
	    {"DeepSeek-V3",                                  {73.80, 1.25e-06, 0.01140}},
	    {"deepseek-ai/DeepSeek-R1-Distill-Qwen-1.5B",   {39.90, 1.80e-07, 0.00500}},
	    // ── Azure OpenAI ──────────────────────────────────────────────────────
	    {"azure/o4-mini-2025-04-16",                     {80.60, 4.40e-06, 0.00920}},
	    {"azure/gpt-4.1-2025-04-14",                     {80.50, 8.00e-06, 0.00760}},
	    {"azure/gpt-4.1-mini-2025-04-14",                {77.20, 1.60e-06, 0.01610}},
	    {"azure/gpt-4o-2024-08-06",                      {74.10, 1.00e-05, 0.00800}},
	    {"azure/gpt-4o-mini-2024-07-18",                 {62.70, 6.00e-07, 0.01590}},
	    {"azure/gpt-4.1-nano-2025-04-14",                {62.30, 4.00e-07, 0.00600}},
	};
	// clang-format on
	return db;
}

// Resolves ModelMetrics for a model_path string.
//
// Match priority:
//   1. Exact key lookup.
//   2. Suffix match — a DB key ends with "/<model_path>" (handles provider-prefixed keys).
//   3. Substring match — model_path is contained in a DB key (handles versioned aliases).
//   4. Conservative defaults for completely unknown models.
inline ModelMetrics LookupModelMetrics(const std::string &model_path) {
	const auto &db = GetModelMetricsDB();

	// 1. Exact
	auto it = db.find(model_path);
	if (it != db.end()) {
		return it->second;
	}

	// 2. Suffix: key == "<provider>/<model_path>"
	for (const auto &kv : db) {
		const auto &key = kv.first;
		if (key.size() > model_path.size() + 1) {
			auto offset = key.size() - model_path.size();
			if (key[offset - 1] == '/' &&
			    key.compare(offset, model_path.size(), model_path) == 0) {
				return kv.second;
			}
		}
	}

	// 3. Substring: e.g. "gpt-4o" matches "gpt-4o-2024-08-06"
	for (const auto &kv : db) {
		if (kv.first.find(model_path) != std::string::npos) {
			return kv.second;
		}
	}

	// 4. Unknown — conservative defaults
	return {50.0, 1.0e-05, 0.01};
}

// ── Scoring policy ─────────────────────────────────────────────────────────────

// Mirrors palimpzest's Policy abstraction.  Weights must sum to 1 but the
// selector normalises them to allow partial specification.
// Constraint fields use 0.0 as "disabled" sentinel:
//   min_quality == 0  →  no lower bound on MMLU-Pro score
//   max_cost    == 0  →  no upper bound on cost
//   max_time    == 0  →  no upper bound on latency
struct ScoringPolicy {
	double quality_weight = 1.0; // weight for MMLU-Pro quality (higher = better)
	double cost_weight    = 0.0; // weight for cost efficiency (lower cost = better)
	double time_weight    = 0.0; // weight for speed (lower latency = better)
	double min_quality    = 0.0; // minimum acceptable MMLU-Pro score (0-100); 0 = disabled
	double max_cost       = 0.0; // maximum USD per output token; 0 = disabled
	double max_time       = 0.0; // maximum seconds per output token; 0 = disabled
};

// ── Core selection algorithm ───────────────────────────────────────────────────

// Selects the best model from `model_paths` using the palimpzest algorithm.
// Returns the index into `model_paths` of the selected model, or 0 on empty input.
inline int SelectOptimalModel(const std::vector<std::string> &model_paths,
                              const ScoringPolicy &policy) {
	if (model_paths.empty()) {
		return 0;
	}

	struct CandidateInfo {
		int    idx;
		double quality;
		double cost;
		double time;
	};

	// Gather metrics for every model
	std::vector<CandidateInfo> all_info;
	all_info.reserve(model_paths.size());
	for (int i = 0; i < static_cast<int>(model_paths.size()); ++i) {
		auto m = LookupModelMetrics(model_paths[i]);
		all_info.push_back({i, m.quality, m.cost, m.time});
	}

	// Apply constraints to build candidate list
	std::vector<CandidateInfo> candidates;
	for (const auto &info : all_info) {
		bool ok = true;
		if (policy.min_quality > 0.0 && info.quality < policy.min_quality) {
			ok = false;
		}
		if (policy.max_cost > 0.0 && info.cost > policy.max_cost) {
			ok = false;
		}
		if (policy.max_time > 0.0 && info.time > policy.max_time) {
			ok = false;
		}
		if (ok) {
			candidates.push_back(info);
		}
	}

	// Fallback: no model satisfies constraints — pick single best by primary metric
	if (candidates.empty()) {
		// Primary metric = whichever weight is largest
		if (policy.quality_weight >= policy.cost_weight &&
		    policy.quality_weight >= policy.time_weight) {
			return static_cast<int>(
			    std::max_element(all_info.begin(), all_info.end(),
			                     [](const CandidateInfo &a, const CandidateInfo &b) {
				                     return a.quality < b.quality;
			                     }) -
			    all_info.begin());
		} else if (policy.cost_weight >= policy.time_weight) {
			return static_cast<int>(
			    std::min_element(all_info.begin(), all_info.end(),
			                     [](const CandidateInfo &a, const CandidateInfo &b) {
				                     return a.cost < b.cost;
			                     }) -
			    all_info.begin());
		} else {
			return static_cast<int>(
			    std::min_element(all_info.begin(), all_info.end(),
			                     [](const CandidateInfo &a, const CandidateInfo &b) {
				                     return a.time < b.time;
			                     }) -
			    all_info.begin());
		}
	}

	// Single candidate — no need to score
	if (candidates.size() == 1) {
		return candidates[0].idx;
	}

	// Min-max normalise across candidates
	double min_q = std::numeric_limits<double>::infinity(),
	       max_q = -std::numeric_limits<double>::infinity();
	double min_c = std::numeric_limits<double>::infinity(),
	       max_c = -std::numeric_limits<double>::infinity();
	double min_t = std::numeric_limits<double>::infinity(),
	       max_t = -std::numeric_limits<double>::infinity();

	for (const auto &c : candidates) {
		min_q = std::min(min_q, c.quality); max_q = std::max(max_q, c.quality);
		min_c = std::min(min_c, c.cost);    max_c = std::max(max_c, c.cost);
		min_t = std::min(min_t, c.time);    max_t = std::max(max_t, c.time);
	}

	auto normalize = [](double val, double lo, double hi, bool invert) -> double {
		if (hi == lo) {
			return 1.0;
		}
		double n = (val - lo) / (hi - lo);
		return invert ? (1.0 - n) : n;
	};

	// Normalise weights so they sum to 1
	double w_sum = policy.quality_weight + policy.cost_weight + policy.time_weight;
	if (w_sum <= 0.0) {
		w_sum = 1.0; // guard against all-zero weights
	}
	double w_q = policy.quality_weight / w_sum;
	double w_c = policy.cost_weight    / w_sum;
	double w_t = policy.time_weight    / w_sum;

	int    best_idx   = candidates[0].idx;
	double best_score = -1.0;

	for (const auto &c : candidates) {
		double n_q = normalize(c.quality, min_q, max_q, /*invert=*/false);
		double n_c = normalize(c.cost,    min_c, max_c, /*invert=*/true);
		double n_t = normalize(c.time,    min_t, max_t, /*invert=*/true);
		double score = w_q * n_q + w_c * n_c + w_t * n_t;
		if (score > best_score) {
			best_score = score;
			best_idx   = c.idx;
		}
	}

	return best_idx;
}

} // namespace duckdb