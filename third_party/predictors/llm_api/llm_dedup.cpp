//
// Created by udeshuk on 5/25/26.
//

#include "llm_api.hpp"
#include "llm_common.hpp"
#include "duckdb/common/string_util.hpp"

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
std::vector<std::vector<float>> LlmApiPredictor::EmbedTexts(const std::vector<std::string> &texts) const {
	if (texts.empty()) {
		return {};
	}

	nlohmann::json request;
	request["model"] = "text-embedding-3-small";
	request["dimensions"] = 256;
	request["input"] = texts;

	const auto response = api->post("embeddings", request);
	if (!response.contains("data") || !response["data"].is_array()) {
		LLM_LOG("EmbedTexts: embeddings call failed or returned no data\n");
		return {};
	}

	std::vector<std::vector<float>> embeddings;
	embeddings.reserve(response["data"].size());
	for (const auto &entry : response["data"]) {
		if (entry.contains("embedding") && entry["embedding"].is_array()) {
			embeddings.push_back(entry["embedding"].get<std::vector<float>>());
		}
	}
	return embeddings;
}

// Groups rows in 'input' by semantic similarity of their info.input_set_names column values.
//
// When a pre-computed embedding column is available (via info.embedding_column_map), the
// stored embedding vectors are read directly from 'input' — no API call is made.
// Otherwise the function falls back to EmbedTexts, and if that also yields nothing, to exact
// string matching.  In all cases clusters[i].key holds the embed_prompt string of the
// representative row and is used for the downstream LLM prompt and cache lookup.
std::vector<TupleCluster> LlmApiPredictor::GroupByClusters(const DataChunk &input, const idx_t rows,
                                                           const PredictInfo &info) const {
	float cluster_threshold = 0.5f;
	std::vector<std::string> prompt_keys;
	prompt_keys.reserve(rows);
	for (idx_t i = 0; i < rows; ++i) {
		prompt_keys.push_back(PromptUtil::embed_prompt(i, input, info, /*is_multi=*/true));
	}

	// --- Attempt 1: read pre-computed embeddings from the input chunk ---
	std::vector<std::vector<float>> embeddings;

	if (!info.embedding_column_map.empty()) {
		// embedding_column_map: source-col-name → embedding-col-name.
		// The binder appends the embedding column to input_set_names, so its index lives in
		// input_mask at the same position.  Find the first available entry.
		idx_t emb_col_idx = DConstants::INVALID_INDEX;
		for (const auto &[src_col, emb_col_name] : info.embedding_column_map) {
			for (idx_t j = 0; j < info.input_set_names.size(); ++j) {
				if (StringUtil::CIEquals(info.input_set_names[j], emb_col_name)) {
					emb_col_idx = info.input_mask[j];
					break;
				}
			}
			if (emb_col_idx != DConstants::INVALID_INDEX) {
				break;
			}
		}

		if (emb_col_idx != DConstants::INVALID_INDEX && emb_col_idx < input.ColumnCount()) {
			// Need a mutable reference for Flatten; we do not modify logical content.
			Vector &arr_col = const_cast<Vector &>(input.data[emb_col_idx]);
			if (arr_col.GetType().id() == LogicalTypeId::ARRAY) {
				// Flatten into a local copy so FlatVector accessors are safe.
				Vector flat(arr_col.GetType());
				flat.Reference(arr_col);
				if (flat.GetVectorType() != VectorType::FLAT_VECTOR) {
					flat.Flatten(rows);
				}

				const auto array_size = ArrayType::GetSize(flat.GetType());
				const Vector &child_vec = ArrayVector::GetEntry(flat);
				const float *child_data = FlatVector::GetData<float>(const_cast<Vector &>(child_vec));
				const auto &validity = FlatVector::Validity(flat);

				embeddings.reserve(rows);
				for (idx_t i = 0; i < rows; ++i) {
					if (validity.RowIsValid(i)) {
						embeddings.emplace_back(child_data + i * array_size,
						                        child_data + (i + 1) * array_size);
					} else {
						embeddings.emplace_back(); // NULL → cosine_similarity returns 0 → new cluster
					}
				}
				LLM_LOG("GroupByClusters: using pre-computed embeddings from input column\n");
			}
		}
	}

	// --- Attempt 2: call EmbedTexts API if no pre-computed embeddings ---
	if (embeddings.size() != rows) {
		std::vector<std::string> texts;
		texts.reserve(rows);
		for (idx_t i = 0; i < rows; ++i) {
			std::stringstream ss;
			for (idx_t j = 0; j < info.input_mask.size(); ++j) {
				if (j > 0) {
					ss << ' ';
				}
				ss << input.GetValue(info.input_mask[j], i).ToSQLString();
			}
			texts.push_back(ss.str());
		}
		embeddings = EmbedTexts(texts);
	}

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