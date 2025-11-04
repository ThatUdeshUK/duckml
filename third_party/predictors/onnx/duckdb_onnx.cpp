#include "duckdb_onnx.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>

namespace duckdb {
ONNXPredictor::ONNXPredictor() : execution_mode(0), intra_tc(0), inter_tc(0) {
}

void ONNXPredictor::Config(const ClientConfig &config, const case_insensitive_map_t<Value> &options) {
	this->batch_size = options.find("batch_size") != options.end() ? IntegerValue::Get(options.at("batch_size"))
	                                                               : config.ml_batch_size;
	this->llm_max_tokens = options.find("llm_max_tokens") != options.end()
	                           ? IntegerValue::Get(options.at("llm_max_tokens"))
	                           : config.llm_max_tokens;
	this->execution_mode = options.find("onnx_execution_mode") != options.end()
	                           ? IntegerValue::Get(options.at("onnx_execution_mode"))
	                           : config.onnx_execution_mode;
	this->intra_tc = options.find("onnx_intra_tc") != options.end() ? IntegerValue::Get(options.at("onnx_intra_tc"))
	                                                                : config.onnx_intra_tc;
	this->inter_tc = options.find("onnx_inter_tc") != options.end() ? IntegerValue::Get(options.at("onnx_inter_tc"))
	                                                                : config.onnx_inter_tc;
}

void ONNXPredictor::Load(ClientContext &client, const std::string &model_path, unique_ptr<PredictStats> &stats) {
#if OPT_TIMING
	const steady_clock::time_point begin = steady_clock::now();
#endif
	this->model_path = model_path;

	if (this->task == PREDICT_LM_TASK)
		tokenizer = FullTokenizer(model_path + "/vocab.txt");

	const std::string instanceName {"model"};
	static Ort::Env env(ORT_LOGGING_LEVEL_WARNING, instanceName.c_str());

	Ort::SessionOptions sessionOptions;
	sessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_ALL);
	if (this->execution_mode == 0) {
		sessionOptions.SetExecutionMode(ORT_SEQUENTIAL);
	} else {
		sessionOptions.SetExecutionMode(ORT_PARALLEL);
	}
	sessionOptions.SetIntraOpNumThreads(this->intra_tc);
	sessionOptions.SetInterOpNumThreads(this->inter_tc);

	std::string final_path = model_path;
	if (this->task == PREDICT_LM_TASK)
		final_path += "/model.onnx";
	Ort::Session session_tmp(env, final_path.c_str(), sessionOptions);
	session = std::move(session_tmp);
#if OPT_TIMING
	const steady_clock::time_point end = steady_clock::now();
	stats->load = duration_cast<std::chrono::microseconds>(end - begin).count();
#endif
}

void ONNXPredictor::Preprocess(const std::string &text, int64_t *input_ids, int64_t *mask, const int offset,
                               const int max_length) const {
	const long start_id = tokenizer.convertTokenToId(L"[CLS]");
	const long end_id = tokenizer.convertTokenToId(L"[SEP]");

	const auto ids_start = input_ids + offset;
	const auto mask_start = mask + offset;
	*ids_start = start_id;
	*mask_start = 1;

	if (const int last_idx = tokenizer.tokenizeToIds(text, ids_start, mask_start, max_length, 1);
	    last_idx == max_length)
		*(ids_start + last_idx - 1) = end_id;
	else {
		*(ids_start + last_idx) = end_id;
		*(mask_start + last_idx) = 1;
	}
}

void ONNXPredictor::Preprocess2(const std::vector<long> &ids, int64_t *input_ids, int64_t *mask, const int offset,
                                const int max_length) const {
	const long start_id = tokenizer.convertTokenToId(L"[CLS]");
	const long end_id = tokenizer.convertTokenToId(L"[SEP]");

	const auto ids_start = input_ids + offset;
	const auto mask_start = mask + offset;
	*ids_start = start_id;
	*mask_start = 1;

	int i = 1;
	for (auto &subToken : ids) {
		*(ids_start + i) = subToken;
		*(mask_start + i) = 1;
		i++;
	}

	if (i == max_length)
		*(ids_start + i - 1) = end_id;
	else {
		*(ids_start + i) = end_id;
		*(mask_start + i) = 1;
	}
}

void ONNXPredictor::PredictLMChunk(ClientContext & /*client*/, DataChunk &input, DataChunk &output, idx_t rows,
                                   const PredictInfo &info, unique_ptr<PredictStats> &stats) {
	int output_size = static_cast<int>(info.result_set_types.size());

	int rounds = rows / batch_size;
	if (rows % batch_size != 0)
		rounds++;

	for (size_t batch = 0; batch < rounds; batch++) {
#if OPT_TIMING
		steady_clock::time_point begin = steady_clock::now();
#endif

		idx_t frow = batch * batch_size;
		idx_t lrow = std::min(frow + batch_size, rows);
		int num_rows = lrow - frow;

#if DYNAMIC_TOKEN_SIZE
		int batch_llm_max_tokens = 0;
		std::vector<std::vector<long>> all_token_ids;
		for (int i = frow; i < lrow; ++i) {
			auto input_str = StringValue::Get(input.GetValue(info.input_mask[0], i));
			auto token_ids = tokenizer.tokenizeToIds(input_str, 510);
			if (int i_count = std::min(static_cast<int>(token_ids.size()) + 2, 512); i_count > batch_llm_max_tokens) {
				batch_llm_max_tokens = i_count;
			}
			all_token_ids.push_back(token_ids);
		}
		llm_max_tokens = batch_llm_max_tokens;
#endif

		std::vector<int64_t> input_ids(num_rows * llm_max_tokens);
		std::vector<int64_t> masks(num_rows * llm_max_tokens);

#if DYNAMIC_TOKEN_SIZE
		for (int i = 0; i < lrow - frow; ++i) {
			int offset = i * llm_max_tokens;
			Preprocess2(all_token_ids[i], input_ids.data(), masks.data(), offset, llm_max_tokens);
		}
#else
		for (int i = frow; i < lrow; ++i) {
			auto input_str = StringValue::Get(input.GetValue(info.input_mask[0], i));
			int offset = (i - frow) * llm_max_tokens;
			Preprocess(input_str, input_ids.data(), masks.data(), offset, llm_max_tokens);
		}
#endif

		std::array<int64_t, 2> inputs_shape_ {num_rows, llm_max_tokens};
		std::array<int64_t, 2> output_shape_ {num_rows, output_size};

		auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
		Ort::Value input_id_tensor_ = Ort::Value::CreateTensor<int64_t>(memory_info, input_ids.data(), input_ids.size(),
		                                                                inputs_shape_.data(), inputs_shape_.size());
		Ort::Value mask_tensor_ = Ort::Value::CreateTensor<int64_t>(memory_info, masks.data(), masks.size(),
		                                                            inputs_shape_.data(), inputs_shape_.size());
		std::array<Ort::Value, 2> input_tensors_ {std::move(input_id_tensor_), std::move(mask_tensor_)};

		std::vector<float> output_data(num_rows * output_size, 0);
		Ort::Value output_tensor_ = Ort::Value::CreateTensor<float>(memory_info, output_data.data(), output_data.size(),
		                                                            output_shape_.data(), output_shape_.size());

		const char *input_names[] = {"input_ids", "attention_mask"};
		const char *output_names[] = {"logits"};

#if OPT_TIMING
		steady_clock::time_point end = steady_clock::now();
		stats->move += duration_cast<std::chrono::microseconds>(end - begin).count();

		begin = steady_clock::now();
#endif

		Ort::RunOptions run_options;
		session.Run(run_options, input_names, input_tensors_.data(), 2, output_names, &output_tensor_, 1);

#if OPT_TIMING
		end = steady_clock::now();
		stats->predict += duration_cast<std::chrono::microseconds>(end - begin).count();

		begin = steady_clock::now();
#endif

		auto out_data_ptr = output_data.data();
		for (idx_t idx = 0; idx < output_size; idx++) {
			float *float_pointer = out_data_ptr + idx;
			data_ptr_t start = data_ptr_cast(float_pointer);

			auto dest = output.data[idx].GetData();
			for (int i = 0; i < num_rows; ++i) {
				*(reinterpret_cast<float *>(dest) + i + frow) = *(reinterpret_cast<float *>(start) + i * output_size);
			}
		}

#if OPT_TIMING
		end = steady_clock::now();
		stats->move_rev += duration_cast<std::chrono::microseconds>(end - begin).count();
#endif
	}
}

void ONNXPredictor::PredictChunk(ClientContext & /*client*/, DataChunk &input, DataChunk &output, idx_t rows,
                                 const PredictInfo &info, unique_ptr<PredictStats> &stats) {
	const int output_size = static_cast<int>(info.result_set_types.size());
	int rounds = rows / batch_size;
	if (rows % batch_size != 0)
		rounds++;

	for (size_t batch = 0; batch < rounds; batch++) {
#if OPT_TIMING
		steady_clock::time_point begin = steady_clock::now();
#endif

		const idx_t frow = batch * batch_size;
		const idx_t lrow = std::min(frow + batch_size, rows);
		const int num_rows = lrow - frow;

		std::array<int64_t, 2> input_shape_ {num_rows, static_cast<long long>(info.input_mask.size())};
		std::array<int64_t, 2> output_shape_ {num_rows, output_size};

#if MOVE_METHOD == ROW_FIRST_PUSH
		std::vector<float> input_data;
		for (int i = frow; i < lrow; ++i) {
			for (auto &j : info.input_mask) {
				input_data.push_back(*((float *)(input.data[j].GetData()) + i));
			}
		}
#endif
#if MOVE_METHOD == ROW_FIRST_COPY
		int cols = info.input_mask.size();
		std::vector<float> input_data(num_rows * cols);
		for (int i = frow; i < lrow; ++i) {
			for (auto &idx : info.input_mask) {
				float *start = (float *)input.data[idx].GetData();
				float *dest = input_data.data() + idx;
				*(dest + i * cols) = *(start + i);
			}
		}
#endif
#if MOVE_METHOD == COL_FIRST_COPY
		const int cols = info.input_mask.size();
		std::vector<float> input_data(num_rows * cols);
		int input_idx = 0;
		for (auto &idx : info.input_mask) {
			const float *start = reinterpret_cast<float *>(input.data[idx].GetData());

			float *dest = input_data.data() + input_idx;
			for (int i = 0; i < num_rows; ++i) {
				*(dest + i * cols) = *(start + i + frow);
			}
			input_idx++;
		}
#endif

		std::vector<float> output_data(num_rows * output_size, 0);

		auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
		Ort::Value input_tensor_ = Ort::Value::CreateTensor<float>(memory_info, input_data.data(), input_data.size(),
		                                                           input_shape_.data(), input_shape_.size());
		Ort::Value output_tensor_ = Ort::Value::CreateTensor<float>(memory_info, output_data.data(), output_data.size(),
		                                                            output_shape_.data(), output_shape_.size());

		const char *input_names[] = {"input"};
		const char *output_names[] = {"output"};

#if OPT_TIMING
		steady_clock::time_point end = steady_clock::now();
		stats->move += duration_cast<std::chrono::microseconds>(end - begin).count();

		begin = steady_clock::now();
#endif

		Ort::RunOptions run_options;
		session.Run(run_options, input_names, &input_tensor_, 1, output_names, &output_tensor_, 1);

#if OPT_TIMING
		end = steady_clock::now();
		stats->predict += duration_cast<std::chrono::microseconds>(end - begin).count();

		begin = steady_clock::now();
#endif

#if MOVE_REV_METHOD == ROW_FIRST_SET
		auto out_data_ptr = output_data.data();
		idx_t idx = frow;
		for (auto i = out_data_ptr; i != out_data_ptr + output_data.size(); ++i) {
			output.SetValue(idx % output_size, idx / output_size, Value(*i));
			idx++;
		}
#endif
#if MOVE_REV_METHOD == COL_FIRST_SET
		auto out_data_ptr = output_data.data();
		for (idx_t idx = 0; idx < output_size; idx++) {
			for (idx_t data_i = 0; data_i < num_rows; ++data_i) {
				float *data = out_data_ptr + idx + (data_i * output_size);
				output.SetValue(idx, frow + data_i, Value(*data));
			}
		}
#endif
#if MOVE_REV_METHOD == ROW_FIRST_COPY
		auto out_data_ptr = output_data.data();
		idx_t idx = frow;
		for (auto i = out_data_ptr; i != out_data_ptr + output_data.size(); ++i) {
			auto dest = output.data[idx % output_size].GetData();
			*((float *)dest + (idx / output_size)) = *i;
			idx++;
		}
#endif
#if MOVE_REV_METHOD == COL_FIRST_COPY
		const auto out_data_ptr = output_data.data();
		for (idx_t idx = 0; idx < output_size; idx++) {
			float *float_pointer = out_data_ptr + idx;
			const data_ptr_t start = data_ptr_cast(float_pointer);

			const auto dest = output.data[idx].GetData();
			for (int i = 0; i < num_rows; ++i) {
				*(reinterpret_cast<float *>(dest) + i + frow) = *(reinterpret_cast<float *>(start) + i * output_size);
			}
		}
#endif

#if OPT_TIMING
		end = steady_clock::now();
		stats->move_rev += duration_cast<std::chrono::microseconds>(end - begin).count();
#endif
	}
}

} // namespace duckdb
