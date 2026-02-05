#include "duckdb/common/vector_operations/vector_operations.hpp"
#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/planner/expression/bound_conjunction_expression.hpp"
#include "duckdb/execution/adaptive_filter.hpp"

#include <iostream>
#include <random>
#include <future>

#define EXECUTE_PARALLEL 0

namespace duckdb {

struct ConjunctionState : public ExpressionState {
	ConjunctionState(const Expression &expr, ExpressionExecutorState &root) : ExpressionState(expr, root) {
		adaptive_filter = make_uniq<AdaptiveFilter>(expr);
	}
	unique_ptr<AdaptiveFilter> adaptive_filter;
};

unique_ptr<ExpressionState> ExpressionExecutor::InitializeState(const BoundConjunctionExpression &expr,
                                                                ExpressionExecutorState &root) {
	auto result = make_uniq<ConjunctionState>(expr, root);
	for (auto &child : expr.children) {
		result->AddChild(*child);
	}

	result->Finalize();
	return std::move(result);
}

void ExpressionExecutor::Execute(const BoundConjunctionExpression &expr, ExpressionState *state,
                                 const SelectionVector *sel, idx_t count, Vector &result) {
	// execute the children
	state->intermediate_chunk.Reset();
	for (idx_t i = 0; i < expr.children.size(); i++) {
		auto &current_result = state->intermediate_chunk.data[i];
		Execute(*expr.children[i], state->child_states[i].get(), sel, count, current_result);
		if (i == 0) {
			// move the result
			result.Reference(current_result);
		} else {
			Vector intermediate(LogicalType::BOOLEAN);
			// AND/OR together
			switch (expr.GetExpressionType()) {
			case ExpressionType::CONJUNCTION_AND:
				VectorOperations::And(current_result, result, intermediate, count);
				break;
			case ExpressionType::CONJUNCTION_OR:
				VectorOperations::Or(current_result, result, intermediate, count);
				break;
			default:
				throw InternalException("Unknown conjunction type!");
			}
			result.Reference(intermediate);
		}
	}
}

idx_t ExpressionExecutor::Select(const BoundConjunctionExpression &expr, ExpressionState *state_p,
                                 const SelectionVector *sel, idx_t count, SelectionVector *true_sel,
                                 SelectionVector *false_sel) {
#if EXECUTE_PARALLEL == 1
	return SelectParallel(expr, state_p, sel, count, true_sel, false_sel);
#endif

	auto &state = state_p->Cast<ConjunctionState>();

	if (expr.GetExpressionType() == ExpressionType::CONJUNCTION_AND) {
		// get runtime statistics
		auto filter_state = state.adaptive_filter->BeginFilter();
		const SelectionVector *current_sel = sel;
		idx_t current_count = count;
		idx_t false_count = 0;

		unique_ptr<SelectionVector> temp_true, temp_false;
		if (false_sel) {
			temp_false = make_uniq<SelectionVector>(STANDARD_VECTOR_SIZE);
		}
		if (!true_sel) {
			temp_true = make_uniq<SelectionVector>(STANDARD_VECTOR_SIZE);
			true_sel = temp_true.get();
		}
		for (idx_t i = 0; i < expr.children.size(); i++) {
			idx_t tcount = Select(*expr.children[state.adaptive_filter->permutation[i]],
			                      state.child_states[state.adaptive_filter->permutation[i]].get(), current_sel,
			                      current_count, true_sel, temp_false.get());
			idx_t fcount = current_count - tcount;
			if (fcount > 0 && false_sel) {
				// move failing tuples into the false_sel
				// tuples passed, move them into the actual result vector
				for (idx_t i = 0; i < fcount; i++) {
					false_sel->set_index(false_count++, temp_false->get_index(i));
				}
			}
			current_count = tcount;
			if (current_count == 0) {
				break;
			}
			if (current_count < count) {
				// tuples were filtered out: move on to using the true_sel to only evaluate passing tuples in subsequent
				// iterations
				current_sel = true_sel;
			}
		}
		// adapt runtime statistics
		state.adaptive_filter->EndFilter(filter_state);
		return current_count;
	} else {
		// get runtime statistics
		auto filter_state = state.adaptive_filter->BeginFilter();

		const SelectionVector *current_sel = sel;
		idx_t current_count = count;
		idx_t result_count = 0;

		unique_ptr<SelectionVector> temp_true, temp_false;
		if (true_sel) {
			temp_true = make_uniq<SelectionVector>(STANDARD_VECTOR_SIZE);
		}
		if (!false_sel) {
			temp_false = make_uniq<SelectionVector>(STANDARD_VECTOR_SIZE);
			false_sel = temp_false.get();
		}
		for (idx_t i = 0; i < expr.children.size(); i++) {
			idx_t tcount = Select(*expr.children[state.adaptive_filter->permutation[i]],
			                      state.child_states[state.adaptive_filter->permutation[i]].get(), current_sel,
			                      current_count, temp_true.get(), false_sel);
			if (tcount > 0) {
				if (true_sel) {
					// tuples passed, move them into the actual result vector
					for (idx_t i = 0; i < tcount; i++) {
						true_sel->set_index(result_count++, temp_true->get_index(i));
					}
				}
				// now move on to check only the non-passing tuples
				current_count -= tcount;
				current_sel = false_sel;
			}
		}
		if (true_sel) {
			true_sel->Sort(result_count);
		}

		// adapt runtime statistics
		state.adaptive_filter->EndFilter(filter_state);
		return result_count;
	}
}

idx_t ExpressionExecutor::AsyncSelect(const Expression &expr, ExpressionState *state, const SelectionVector *sel,
									  idx_t count, SelectionVector *true_sel, SelectionVector *false_sel) {
	idx_t out = Select(expr, state, sel, count, true_sel, false_sel);
	// for (idx_t j = 0; j < out; j++) {
	// 	std::cout << "AsyncSelect- " << " idx- " << j << ", mapping- " << true_sel->get_index(j) << std::endl;
	// }
	return out;
}

idx_t ExpressionExecutor::SelectParallel(const BoundConjunctionExpression &expr, ExpressionState *state_p,
                                 const SelectionVector *sel, idx_t count, SelectionVector *true_sel,
                                 SelectionVector *false_sel) {
	auto &state = state_p->Cast<ConjunctionState>();

	if (expr.GetExpressionType() == ExpressionType::CONJUNCTION_AND) {
		// get runtime statistics
		auto filter_state = state.adaptive_filter->BeginFilter();
		const SelectionVector *current_sel = sel;
		idx_t false_count = 0;

		unique_ptr<SelectionVector> temp_true, temp_false;
		if (false_sel) {
			temp_false = make_uniq<SelectionVector>(STANDARD_VECTOR_SIZE);
		}
		if (!true_sel) {
			temp_true = make_uniq<SelectionVector>(STANDARD_VECTOR_SIZE);
			true_sel = temp_true.get();
		}

		vector<std::future<idx_t>> futures;
		// vector<idx_t> futures_i;
		vector<unique_ptr<SelectionVector>> children_true;
		for (idx_t i = 0; i < expr.children.size(); i++) {
			children_true.push_back(make_uniq<SelectionVector>(STANDARD_VECTOR_SIZE));
		}

		vector<SelectionVector> children_false{expr.children.size(), SelectionVector{STANDARD_VECTOR_SIZE}};
		vector<idx_t> true_counts{expr.children.size(), 0};

		for (idx_t i = 0; i < expr.children.size(); i++) {
			// futures_i.push_back(AsyncSelect(*expr.children[state.adaptive_filter->permutation[i]],
			// 								state.child_states[state.adaptive_filter->permutation[i]].get(),
			// 								current_sel, count, children_true[i].get(), &children_false[i]));

			// for (idx_t j = 0; j < futures_i[i]; j++) {
			// 	std::cout << "InLoopFilter- " << i << " , idx- " << j << ", mapping- " << children_true[i]->get_index(j) << std::endl;
			// }
			futures.push_back(std::async(std::launch::async, &ExpressionExecutor::AsyncSelect,
								this, std::cref(*expr.children[state.adaptive_filter->permutation[i]]),
								state.child_states[state.adaptive_filter->permutation[i]].get(),
								std::cref(current_sel), count, children_true[i].get(), &children_false[i]));
		}

		for (idx_t i = 0; i < expr.children.size(); i++) {
			// idx_t tcount = futures_i[i];
			idx_t tcount = futures[i].get();
			idx_t fcount = count - tcount;
			if (fcount > 0 && false_sel) {
				// move failing tuples into the false_sel
				// tuples passed, move them into the actual result vector
				for (idx_t j = 0; j < fcount; j++) {
					false_sel->set_index(false_count++, children_false[i].get_index(j));
				}
			}
			true_counts[i] = tcount;
			// std::cout << "Filtered: " << tcount << std::endl;
		}
		idx_t true_count = 0;
		idx_t lp = 0;
		idx_t rp = 0;
		while (lp < true_counts[0] && rp < true_counts[1]) {
			idx_t lidx = children_true[0]->get_index(lp);
			idx_t ridx = children_true[1]->get_index(rp);
			if (lidx == ridx) {
				// std::cout << "Match: " << lidx << std::endl;
				true_sel->set_index(true_count, lidx);
				true_count++;
				lp++;
				rp++;
			} else if (lidx < ridx) {
				lp++;
			} else if (ridx < lidx) {
				rp++;
			}
		}
		// for (idx_t i = 0; i < children_true.size(); i++) {
		// 	for (idx_t j = 0; j < true_counts[i]; j++) {
		// 		std::cout << "Filter- " << i << " , idx- " << j << ", mapping- " << children_true[i]->get_index(j) << std::endl;
		// 		true_sel->set_index(true_count, children_true[i]->get_index(j));
		// 		true_count++;
		// 	}
		// }
		// adapt runtime statistics
		state.adaptive_filter->EndFilter(filter_state);
		return true_count;
	} else {
		throw std::runtime_error("Expression executor does not support select_parallel() with !AND");
	}
}

} // namespace duckdb
