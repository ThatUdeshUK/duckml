#include "duckdb/common/numeric_utils.hpp"
#include "duckdb/common/types/column/column_data_collection.hpp"
#include "duckdb/common/types/list_segment.hpp"
#include "duckdb/function/aggregate_function.hpp"
#include "duckdb/function/function_binder.hpp"
#include "duckdb/planner/expression/bound_window_expression.hpp"
#include "duckdb/storage/buffer_manager.hpp"
#include "duckdb/planner/expression/bound_aggregate_expression.hpp"
#include "duckdb/planner/expression/bound_predict_expression.hpp"
#include "duckdb/planner/expression/bound_constant_expression.hpp"
#include "duckdb/parser/expression_map.hpp"

namespace duckdb {

struct PredictAggregateBindData : public FunctionData {
	using Expressions = vector<unique_ptr<Expression>>;
	using BindInfoPtr = unique_ptr<FunctionData>;

	PredictAggregateBindData(ClientContext &context_p, Expressions &children)
	    : context(context_p) {
		data_types.reserve(children.size());
		data_funcs.reserve(children.size());
		for (const auto &child : children) {
			data_types.emplace_back(child->return_type);
			ListSegmentFunctions funcs;
			GetSegmentDataFunctions(funcs, data_types.back());
			data_funcs.emplace_back(std::move(funcs));
		}
	}

	PredictAggregateBindData(const PredictAggregateBindData &other)
	    : context(other.context), data_types(other.data_types), data_funcs(other.data_funcs) {
	}

	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<PredictAggregateBindData>(*this);
	}

	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<PredictAggregateBindData>();
		return true;
	}

	ClientContext &context;
	vector<LogicalType> data_types;
	vector<ListSegmentFunctions> data_funcs;
};

struct PredictAggregateState {
	// Linked list equivalent of DataChunk
	using LinkedLists = vector<LinkedList>;
	using LinkedChunkFunctions = vector<ListSegmentFunctions>;

	//! Capacities of the various levels of buffering
	static const idx_t CHUNK_CAPACITY = STANDARD_VECTOR_SIZE;
	static const idx_t LIST_CAPACITY = MinValue<idx_t>(16, CHUNK_CAPACITY);

	PredictAggregateState() : count(0), nsel(0), offset(0) {
	}

	static inline void InitializeLinkedList(LinkedLists &linked, const vector<LogicalType> &types) {
		if (linked.empty() && !types.empty()) {
			linked.resize(types.size(), LinkedList());
		}
	}

	inline void InitializeLinkedLists(const PredictAggregateBindData &predict_bind) {
		InitializeLinkedList(data_linked, predict_bind.data_types);
	}

	static inline void InitializeChunk(Allocator &allocator, unique_ptr<DataChunk> &chunk,
	                                   const vector<LogicalType> &types) {
		if (!chunk && !types.empty()) {
			chunk = make_uniq<DataChunk>();
			chunk->Initialize(allocator, types);
		}
	}

	void InitializeChunks(const PredictAggregateBindData &predict_bind) {
		// Lazy instantiation of the buffer chunks
		auto &allocator = BufferManager::GetBufferManager(predict_bind.context).GetBufferAllocator();
		InitializeChunk(allocator, data_chunk, predict_bind.data_types);
	}

	static inline void FlushLinkedList(const LinkedChunkFunctions &funcs, LinkedLists &linked, DataChunk &chunk) {
		idx_t total_count = 0;
		for (column_t i = 0; i < linked.size(); ++i) {
			funcs[i].BuildListVector(linked[i], chunk.data[i], total_count);
			chunk.SetCardinality(linked[i].total_capacity);
		}
	}

	void FlushLinkedLists(const PredictAggregateBindData &predict_bind) {
		InitializeChunks(predict_bind);
		FlushLinkedList(predict_bind.data_funcs, data_linked, *data_chunk);
	}

	void InitializeCollections(const PredictAggregateBindData &predict_bind) {
		data = make_uniq<ColumnDataCollection>(predict_bind.context, predict_bind.data_types);
		data_append = make_uniq<ColumnDataAppendState>();
		data->InitializeAppend(*data_append);
	}

	void FlushChunks(const PredictAggregateBindData &predict_bind) {
		D_ASSERT(data_chunk);
		data->Append(*data_append, *data_chunk);
		data_chunk->Reset();
	}

	void Resize(const PredictAggregateBindData &predict_bind, idx_t n) {
		count = n;

		//	Establish the current buffering
		if (count <= LIST_CAPACITY) {
			InitializeLinkedLists(predict_bind);
		}

		if (count > LIST_CAPACITY && !data_chunk && !data) {
			FlushLinkedLists(predict_bind);
		}

		if (count > CHUNK_CAPACITY && !data) {
			InitializeCollections(predict_bind);
			FlushChunks(predict_bind);
		}
	}

	static void LinkedAppend(const LinkedChunkFunctions &functions, ArenaAllocator &allocator, DataChunk &input,
	                         LinkedLists &linked, SelectionVector &sel, idx_t nsel) {
		const auto count = input.size();
		for (column_t c = 0; c < input.ColumnCount(); ++c) {
			auto &func = functions[c];
			auto &linked_list = linked[c];
			RecursiveUnifiedVectorFormat input_data;
			Vector::RecursiveToUnifiedFormat(input.data[c], count, input_data);
			for (idx_t i = 0; i < nsel; ++i) {
				idx_t sidx = sel.get_index(i);
				func.AppendRow(allocator, linked_list, input_data, sidx);
			}
		}
	}

	static void LinkedAbsorb(LinkedLists &source, LinkedLists &target) {
		D_ASSERT(source.size() == target.size());
		for (column_t i = 0; i < source.size(); ++i) {
			auto &src = source[i];
			if (!src.total_capacity) {
				break;
			}

			auto &tgt = target[i];
			if (!tgt.total_capacity) {
				tgt = src;
			} else {
				// append the linked list
				tgt.last_segment->next = src.first_segment;
				tgt.last_segment = src.last_segment;
				tgt.total_capacity += src.total_capacity;
			}
		}
	}

	void Update(const AggregateInputData &aggr_input_data, DataChunk &data_input) {
		const auto &predict_bind = aggr_input_data.bind_data->Cast<PredictAggregateBindData>();
		Resize(predict_bind, count + data_input.size());

		sel.Initialize(nullptr);
		nsel = data_input.size();

		if (data) {
			//	Using collections
			data->Append(*data_append, data_input);
		} else if (data_chunk) {
			//	Still using data chunks
			data_chunk->Append(data_input);
		} else {
			//	Still using linked lists
			LinkedAppend(predict_bind.data_funcs, aggr_input_data.allocator, data_input, data_linked, sel, nsel);
		}

		nsel = 0;
		offset = 0;
	}

	void Swap(PredictAggregateState &other) {
		std::swap(count, other.count);
		std::swap(data_chunk, other.data_chunk);
		std::swap(data_linked, other.data_linked);
	}

	void Absorb(const PredictAggregateBindData &predict_bind, PredictAggregateState &other) {
		if (!other.count) {
			return;
		} else if (!count) {
			Swap(other);
			return;
		}

		//	Change to a state large enough for all the data
		Resize(predict_bind, count + other.count);

		//	3x3 matrix.
		//	We can simplify the logic a bit because the target is already set for the final capacity
		if (!data_chunk) {
			//	If the combined count is still linked lists,
			//	then just move the pointers.
			//	Note that this assumes ArenaAllocator is shared and the memory will not vanish under us.
			LinkedAbsorb(other.data_linked, data_linked);
			other.Reset();
			return;
		}

		if (!other.data_chunk) {
			other.FlushLinkedLists(predict_bind);
		}

	
		if (!data) {
			//	Still using chunks, which means the source is using chunks or lists
			D_ASSERT(data_chunk);
			D_ASSERT(other.data_chunk);
			data_chunk->Append(*other.data_chunk);
		} else {
			// Using collections, so source could be using anything.
			if (other.data) {
				data->Combine(*other.data);
			} else {
				data->Append(*other.data_chunk);
			}
		}
		//	Free all memory as we have absorbed it.
		other.Reset();
	}

	void OutputBuffer(DataChunk &output) {
		for (column_t col_idx = 0; col_idx < data_chunk->ColumnCount(); ++col_idx) {
			output.data[col_idx + 1].Reference(data_chunk->data[col_idx]);
		}
		output.SetCardinality(*data_chunk);
	}

	void Finalize(const PredictAggregateBindData &predict_bind, DataChunk &output) {
		if (data) {
			ColumnDataScanState data_state;
			data->InitializeScan(data_state);
			for (data_chunk->Reset(); data->Scan(data_state, *data_chunk); data_chunk->Reset()) {
				// Stort was applied heree
			}
		} else {
			//	Force chunks so we can sort
			if (!data_chunk) {
				FlushLinkedLists(predict_bind);
			}

			// Sort was applied here
			OutputBuffer(output);
			// local_sort.SinkChunk(output, *arg_chunk);
		}

		Reset();
	}

	void Reset() {
		//	Release all memory
		data.reset();
		data_chunk.reset();
		data_linked.clear();
		count = 0;
	}

	idx_t count;

	unique_ptr<ColumnDataCollection> data;
	unique_ptr<ColumnDataAppendState> data_append;

	unique_ptr<DataChunk> data_chunk;

	LinkedLists data_linked;

	// Selection for scattering
	SelectionVector sel;
	idx_t nsel;
	idx_t offset;
};

struct PredictAggregateFunction {
	template <typename STATE>
	static void Initialize(STATE &state) {
		new (&state) STATE();
	}

	template <typename STATE>
	static void Destroy(STATE &state, AggregateInputData &aggr_input_data) {
		state.~STATE();
	}

	static void ProjectInputs(Vector inputs[], const PredictAggregateBindData &predict_bind, idx_t input_count,
	                          idx_t count, DataChunk &data_input) {
		idx_t col = 0;

		data_input.InitializeEmpty(predict_bind.data_types);
		for (auto &dst : data_input.data) {
			dst.Reference(inputs[col++]);
		}
		data_input.SetCardinality(count);
	}

	static void SimpleUpdate(Vector inputs[], AggregateInputData &aggr_input_data, idx_t input_count, data_ptr_t state,
	                         idx_t count) {
		const auto predict_bind = aggr_input_data.bind_data->Cast<PredictAggregateBindData>();
		DataChunk data_input;
		ProjectInputs(inputs, predict_bind, input_count, count, data_input);

		const auto order_state = reinterpret_cast<PredictAggregateState *>(state);
		order_state->Update(aggr_input_data, data_input);
	}

	static void ScatterUpdate(Vector inputs[], AggregateInputData &aggr_input_data, idx_t input_count, Vector &states,
	                          idx_t count) {
		if (!count) {
			return;
		}

		// Append the arguments to the two sub-collections
		const auto &predict_bind = aggr_input_data.bind_data->Cast<PredictAggregateBindData>();
		DataChunk data_inputs;
		ProjectInputs(inputs, predict_bind, input_count, count, data_inputs);

		// We have to scatter the chunks one at a time
		// so build a selection vector for each one.
		UnifiedVectorFormat svdata;
		states.ToUnifiedFormat(count, svdata);

		// Size the selection vector for each state.
		auto sdata = UnifiedVectorFormat::GetDataNoConst<PredictAggregateState *>(svdata);
		for (idx_t i = 0; i < count; ++i) {
			auto sidx = svdata.sel->get_index(i);
			auto order_state = sdata[sidx];
			order_state->nsel++;
		}

		// Build the selection vector for each state.
		vector<sel_t> sel_data(count);
		idx_t start = 0;
		for (idx_t i = 0; i < count; ++i) {
			auto sidx = svdata.sel->get_index(i);
			auto order_state = sdata[sidx];
			if (!order_state->offset) {
				//	First one
				order_state->offset = start;
				order_state->sel.Initialize(sel_data.data() + order_state->offset);
				start += order_state->nsel;
			}
			sel_data[order_state->offset++] = UnsafeNumericCast<sel_t>(sidx);
		}

		// Append nonempty slices to the arguments
		// for (idx_t i = 0; i < count; ++i) {
		// 	auto sidx = svdata.sel->get_index(i);
		// 	auto order_state = sdata[sidx];
		// 	if (!order_state->nsel) {
		// 		continue;
		// 	}

		// 	order_state->UpdateSlice(aggr_input_data, data_inputs, arg_inputs);
		// }
	}

	template <class STATE, class OP>
	static void Combine(const STATE &source, STATE &target, AggregateInputData &aggr_input_data) {
		auto &predict_bind = aggr_input_data.bind_data->Cast<PredictAggregateBindData>();
		auto &other = const_cast<STATE &>(source); // NOLINT: absorb explicitly allows destruction
		target.Absorb(predict_bind, other);
	}

	static void Window(AggregateInputData &aggr_input_data, const WindowPartitionInput &partition,
	                   const_data_ptr_t g_state, data_ptr_t l_state, const SubFrames &subframes, Vector &result,
	                   idx_t rid) {
		throw InternalException("Sorted aggregates should not be generated for window clauses");
	}

	static void Finalize(Vector &states, AggregateInputData &aggr_input_data, Vector &result, idx_t count,
	                     const idx_t offset) {
		auto &predict_bind = aggr_input_data.bind_data->Cast<PredictAggregateBindData>();
		auto &context = predict_bind.context;
		RowLayout payload_layout;
		payload_layout.Initialize(predict_bind.data_types);

		auto &buffer_allocator = BufferManager::GetBufferManager(predict_bind.context).GetBufferAllocator();
		DataChunk chunk;
		chunk.Initialize(buffer_allocator, predict_bind.data_types);
		DataChunk sliced;
		sliced.Initialize(buffer_allocator, predict_bind.data_types);

		//	 Reusable inner state
		// auto &aggr = predict_bind.function;
		// vector<data_t> agg_state(aggr.state_size(aggr));
		// Vector agg_state_vec(Value::POINTER(CastPointerToValue(agg_state.data())));

		// // State variables
		// auto bind_info = predict_bind.bind_info.get();
		// AggregateInputData aggr_bind_info(bind_info, aggr_input_data.allocator);

		// // Inner aggregate APIs
		// auto initialize = aggr.initialize;
		// auto destructor = aggr.destructor;
		// auto simple_update = aggr.simple_update;
		// auto update = aggr.update;
		// auto finalize = aggr.finalize;

		// auto sdata = FlatVector::GetData<PredictAggregateState *>(states);

		// vector<idx_t> state_unprocessed(count, 0);
		// for (idx_t i = 0; i < count; ++i) {
		// 	state_unprocessed[i] = sdata[i]->count;
		// }

		// Sort the input payloads on (state_idx ASC, orders)
		// vector<BoundOrderByNode> orders;
		// orders.emplace_back(BoundOrderByNode(OrderType::ASCENDING, OrderByNullType::NULLS_FIRST,
		//                                      make_uniq<BoundConstantExpression>(Value::USMALLINT(0))));
		// for (const auto &order : predict_bind.orders) {
		// 	orders.emplace_back(order.Copy());
		// }

		// auto global_sort = make_uniq<GlobalSortState>(context, orders, payload_layout);
		// global_sort->external = predict_bind.external;
		// auto local_sort = make_uniq<LocalSortState>();
		// local_sort->Initialize(*global_sort, global_sort->buffer_manager);

	// 	DataChunk prefixed;
	// 	prefixed.Initialize(buffer_allocator, global_sort->sort_layout.logical_types);

	// 	//	Go through the states accumulating values to sort until we hit the sort threshold
	// 	idx_t unsorted_count = 0;
	// 	idx_t sorted = 0;
	// 	for (idx_t finalized = 0; finalized < count;) {
	// 		if (unsorted_count < predict_bind.threshold) {
	// 			auto state = sdata[finalized];
	// 			prefixed.Reset();
	// 			prefixed.data[0].Reference(Value::USMALLINT(UnsafeNumericCast<uint16_t>(finalized)));
	// 			state->Finalize(predict_bind, prefixed, *local_sort);
	// 			unsorted_count += state_unprocessed[finalized];

	// 			// Go to the next aggregate unless this is the last one
	// 			if (++finalized < count) {
	// 				continue;
	// 			}
	// 		}

	// 		//	If they were all empty (filtering) flush them
	// 		//	(This can only happen on the last range)
	// 		if (!unsorted_count) {
	// 			break;
	// 		}

	// 		//	Sort all the data
	// 		global_sort->AddLocalState(*local_sort);
	// 		global_sort->PrepareMergePhase();
	// 		while (global_sort->sorted_blocks.size() > 1) {
	// 			global_sort->InitializeMergeRound();
	// 			MergeSorter merge_sorter(*global_sort, global_sort->buffer_manager);
	// 			merge_sorter.PerformInMergeRound();
	// 			global_sort->CompleteMergeRound(false);
	// 		}

	// 		auto scanner = make_uniq<PayloadScanner>(*global_sort);
	// 		initialize(aggr, agg_state.data());
	// 		while (scanner->Remaining()) {
	// 			chunk.Reset();
	// 			scanner->Scan(chunk);
	// 			idx_t consumed = 0;

	// 			// Distribute the scanned chunk to the aggregates
	// 			while (consumed < chunk.size()) {
	// 				//	Find the next aggregate that needs data
	// 				for (; !state_unprocessed[sorted]; ++sorted) {
	// 					// Finalize a single value at the next offset
	// 					agg_state_vec.SetVectorType(states.GetVectorType());
	// 					finalize(agg_state_vec, aggr_bind_info, result, 1, sorted + offset);
	// 					if (destructor) {
	// 						destructor(agg_state_vec, aggr_bind_info, 1);
	// 					}

	// 					initialize(aggr, agg_state.data());
	// 				}
	// 				const auto input_count = MinValue(state_unprocessed[sorted], chunk.size() - consumed);
	// 				for (column_t col_idx = 0; col_idx < chunk.ColumnCount(); ++col_idx) {
	// 					sliced.data[col_idx].Slice(chunk.data[col_idx], consumed, consumed + input_count);
	// 				}
	// 				sliced.SetCardinality(input_count);

	// 				// These are all simple updates, so use it if available
	// 				if (simple_update) {
	// 					simple_update(sliced.data.data(), aggr_bind_info, sliced.data.size(), agg_state.data(),
	// 					              sliced.size());
	// 				} else {
	// 					// We are only updating a constant state
	// 					agg_state_vec.SetVectorType(VectorType::CONSTANT_VECTOR);
	// 					update(sliced.data.data(), aggr_bind_info, sliced.data.size(), agg_state_vec, sliced.size());
	// 				}

	// 				consumed += input_count;
	// 				state_unprocessed[sorted] -= input_count;
	// 			}
	// 		}

	// 		//	Finalize the last state for this sort
	// 		agg_state_vec.SetVectorType(states.GetVectorType());
	// 		finalize(agg_state_vec, aggr_bind_info, result, 1, sorted + offset);
	// 		if (destructor) {
	// 			destructor(agg_state_vec, aggr_bind_info, 1);
	// 		}
	// 		++sorted;

	// 		//	Stop if we are done
	// 		if (finalized >= count) {
	// 			break;
	// 		}

	// 		//	Create a new sort
	// 		scanner.reset();
	// 		global_sort = make_uniq<GlobalSortState>(context, orders, payload_layout);
	// 		global_sort->external = predict_bind.external;
	// 		local_sort = make_uniq<LocalSortState>();
	// 		local_sort->Initialize(*global_sort, global_sort->buffer_manager);
	// 		unsorted_count = 0;
	// 	}

	// 	for (; sorted < count; ++sorted) {
	// 		initialize(aggr, agg_state.data());

	// 		// Finalize a single value at the next offset
	// 		agg_state_vec.SetVectorType(states.GetVectorType());
	// 		finalize(agg_state_vec, aggr_bind_info, result, 1, sorted + offset);

	// 		if (destructor) {
	// 			destructor(agg_state_vec, aggr_bind_info, 1);
	// 		}
	// 	}

	// 	result.Verify(count);
	}
};

unique_ptr<BoundAggregateExpression> FunctionBinder::BindAggregatePredict(PredictExpression &expr,
																		  vector<unique_ptr<Expression>> children) {
	
	auto predict_bind = make_uniq<PredictAggregateBindData>(context, children);

	vector<LogicalType> arguments;
	arguments.reserve(children.size());
	for (const auto &child : children) {
		arguments.emplace_back(child->return_type);
	}

	// Replace the aggregate with the wrapper
	AggregateFunction predict_aggregate(
	    "llm_agg", arguments, LogicalType::VARCHAR, AggregateFunction::StateSize<PredictAggregateState>,
	    AggregateFunction::StateInitialize<PredictAggregateState, PredictAggregateFunction,
	                                       AggregateDestructorType::LEGACY>,
	    PredictAggregateFunction::ScatterUpdate,
	    AggregateFunction::StateCombine<PredictAggregateState, PredictAggregateFunction>,
	    PredictAggregateFunction::Finalize, FunctionNullHandling::DEFAULT_NULL_HANDLING, PredictAggregateFunction::SimpleUpdate, nullptr,
	    AggregateFunction::StateDestroy<PredictAggregateState, PredictAggregateFunction>, nullptr,
	    PredictAggregateFunction::Window);

	return make_uniq<BoundAggregateExpression>(std::move(predict_aggregate), std::move(children), nullptr,
	                                           std::move(predict_bind), AggregateType::NON_DISTINCT);
};

} // namespace duckdb
