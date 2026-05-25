//
// Created by udeshuk on 5/25/26.
//

#include "llm_api.hpp"
#include "llm_common.hpp"

#include <math.h>

namespace duckdb {

static float cosine_similarity(const std::vector<float> &a, const std::vector<float> &b) {
	float dot = 0.0f, na = 0.0f, nb = 0.0f;
	const size_t n = std::min(a.size(), b.size());
	for (size_t k = 0; k < n; ++k) {
		dot += a[k] * b[k];
		na  += a[k] * a[k];
		nb  += b[k] * b[k];
	}
	const float denom = std::sqrt(na) * std::sqrt(nb);
	return denom > 0.0f ? dot / denom : 0.0f;
}

void LlmApiPredictor::PropagateSingleResult(const std::string &llm_out, const idx_t unprocessed_idx,
											 map<string, vector<idx_t>> &tuple_id_map,
											 DataChunk &output, const PredictInfo &info) {
#if LLM_USE_CLUSTER
	const auto cluster_it = std::next(tuple_id_map.begin(), static_cast<std::ptrdiff_t>(unprocessed_idx));
	if (use_cache) {
		cache[cluster_it->first] = llm_out;
	}
	for (const idx_t tuple_id : cluster_it->second) {
		prompt_util.extract_row_data(llm_out, tuple_id, output, info);
	}
#else
	if (use_cache) {
		const auto unprocessed_row = std::next(tuple_id_map.begin(),
											   static_cast<std::ptrdiff_t>(unprocessed_idx));
		cache[unprocessed_row->first] = llm_out;
		for (const auto &tuple_id : unprocessed_row->second) {
			prompt_util.extract_row_data(llm_out, tuple_id, output, info);
		}
	} else {
		prompt_util.extract_row_data(llm_out, unprocessed_idx, output, info);
	}
#endif
}

// Calls the embeddings endpoint for 'texts' and returns one float vector per entry.
// Returns an empty vector when cluster_embed_model is unset or the call fails.
std::vector<std::vector<float>> EmbedTexts(const std::vector<std::string> &texts) {
	std::vector<std::vector<float>> embeddings;
	embeddings.reserve(texts.size());
	// TODO: Implement the actual embedding logic
	return embeddings;
}

// Groups rows in 'input' by semantic similarity of their info.input_set_names column values.
//
// When cluster_embed_model is set, each row's input columns are concatenated into a plain-text
// string and sent to the embeddings API.  Rows are then assigned to clusters via greedy
// cosine-similarity search: a row joins the nearest existing cluster whose representative
// embedding exceeds cluster_threshold; otherwise a new cluster is started.
//
// When cluster_embed_model is empty the function falls back to exact string matching (no API
// call).  In both cases clusters[i].key holds the embed_prompt string of the representative
// row and is used for the downstream LLM prompt and cache lookup.
std::vector<TupleCluster> LlmApiPredictor::GroupByClusters(const DataChunk &input, const idx_t rows,
                                                           const PredictInfo &info) const {
	// Build per-row text (for embedding) and formatted key (for LLM prompt + cache).
	float cluster_threshold = 0.7f;
	std::vector<std::string> texts;
	std::vector<std::string> prompt_keys;
	texts.reserve(rows);
	prompt_keys.reserve(rows);

	for (idx_t i = 0; i < rows; ++i) {
		// Plain concatenation of column values used as embedding input.
		std::stringstream ss;
		for (idx_t j = 0; j < info.input_mask.size(); ++j) {
			if (j > 0) {
				ss << ' ';
			}
			ss << input.GetValue(info.input_mask[j], i).ToSQLString();
		}
		texts.push_back(ss.str());
		prompt_keys.push_back(PromptUtil::embed_prompt(i, input, info, /*is_multi=*/true));
	}

	// Try to get vector embeddings; fall through to exact match on failure.
	const auto embeddings = EmbedTexts(texts);
	const bool use_embeddings = (embeddings.size() == rows);

	if (use_embeddings) {
		LLM_LOG("GroupByClusters: using embedding-based clustering (threshold=" +
		        std::to_string(cluster_threshold) + ")\n");

		std::vector<TupleCluster> clusters;
		// Store one centroid embedding per cluster (representative's embedding).
		std::vector<std::vector<float>> centroids;

		for (idx_t i = 0; i < rows; ++i) {
			idx_t best_cluster = static_cast<idx_t>(clusters.size()); // sentinel = new cluster
			float best_sim = cluster_threshold;

			for (idx_t ci = 0; ci < static_cast<idx_t>(clusters.size()); ++ci) {
				const float sim = cosine_similarity(embeddings[i], centroids[ci]);
				if (sim > best_sim) {
					best_sim = sim;
					best_cluster = ci;
				}
			}

			if (best_cluster < static_cast<idx_t>(clusters.size())) {
				clusters[best_cluster].rows.push_back(i);
			} else {
				clusters.push_back({prompt_keys[i], {i}});
				centroids.push_back(embeddings[i]);
			}
		}

		LLM_LOG("GroupByClusters: " + std::to_string(rows) + " rows → " +
		        std::to_string(clusters.size()) + " clusters\n");
		return clusters;
	}

	// Exact-match fallback: group rows with identical prompt keys.
	LLM_LOG("GroupByClusters: using exact-match clustering (no embed model)\n");
	std::map<std::string, idx_t> key_to_idx;
	std::vector<TupleCluster> clusters;

	for (idx_t i = 0; i < rows; ++i) {
		const std::string &key = prompt_keys[i];
		auto [it, inserted] = key_to_idx.emplace(key, static_cast<idx_t>(clusters.size()));
		if (inserted) {
			clusters.push_back({key, {i}});
		} else {
			clusters[it->second].rows.push_back(i);
		}
	}
	return clusters;
}

}