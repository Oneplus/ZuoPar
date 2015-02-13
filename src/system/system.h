#ifndef __ZUOPAR_SYSTEM_SYSTEM_H__
#define __ZUOPAR_SYSTEM_SYSTEM_H__

#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <boost/tuple/tuple.hpp>
#include "settings.h"
#include "system/options.h"
#include "utils/logging.h"
#include "types/internal/packed_scores.h"

namespace ZuoPar {

template <
  class _ActionType,
  class _StateType,
  class _ScoreContextType,
  class _ModelType
>
class TransitionSystem {
public:
  //! Define the type of const decode result.
  typedef std::pair<const _StateType*, const _StateType*> const_decode_result_t;

  //! Define the type of regular decode result.
  typedef std::pair<_StateType*, _StateType*> decode_result_t;

  //! Define a sequence of actions.
  typedef std::vector<_ActionType> action_sequence_t;

protected:
  //! Define a set of actions.
  typedef std::vector<_ActionType>  action_collection_t;

  //! Define the type of header element of lattice.
  typedef std::vector<_StateType*>  lattice_head_list_t;

  //! Define the type of element volumn of lattice.
  typedef std::vector<int> lattice_size_list_t;

  //! Define the mapping between action and it's packed score.
  typedef PackedScores<_ActionType> packed_score_t;

  //! Define the (source.state, action, score) tuple.
  typedef boost::tuples::tuple<const _StateType*, _ActionType, floatval_t> scored_transition_t;

  //! The comparsion function between two scored transition.
  static bool ScoredTransitionMore(const scored_transition_t& x,
      const scored_transition_t& y) {
    return x.template get<2>() > y.template get<2>();
  }

protected:
  //! The pointer to the model, used to score the certain transition.
  _ModelType* model;

  //! The header of each row in lattice.
  lattice_head_list_t lattice_heads;

  //! The size of each row in lattice.
  lattice_size_list_t lattice_size;

  //! The size of the beam.
  int beam_size;

  //! The step
  int step;

  //! Use to specify if use averaged parameter.
  bool use_avg;

  //! Use to specify if use early update strategy.
  UpdateStrategy update_strategy;

protected:
  //! The cached possible actions.
  action_collection_t possible_actions;

  //! The cached packed score.
  packed_score_t packed_scores;

  //! The k-best candidate transitions.
  scored_transition_t* candidate_transitions;
public:
  /**
   * The allocator for transition system.
   *
   *  @param[in]  beam_size_        The beam size.
   *  @param[in]  update_strategy_  The update strategy: none, early update or
   *                                max violation.
   *  @param[in]  model_            The model pointer.
   */
  TransitionSystem(
      int beam_size_,
      bool use_avg_,
      UpdateStrategy update_strategy_,
      _ModelType* model_)
    : beam_size(beam_size_),
    use_avg(use_avg_),
    step(-1),
    update_strategy(update_strategy_),
    model(model_) {
    candidate_transitions = new scored_transition_t[beam_size];
  }

  //! The deallocator
  ~TransitionSystem() {
    for (size_t i = 0; i < lattice_heads.size(); ++ i) {
      if (lattice_heads[i]) {
        delete [](lattice_heads[i]);
        lattice_heads[i] = 0;
      }
    }

    delete [](candidate_transitions);
  }

  /**
   * The main process of decoding.
   *
   *  @param[in]  initial_state   The initial state.
   *  @param[in]  gold_actions    The gold actions.
   *  @param[in]  max_nr_actions  The maximum number of transition actions.
   *  @return     decode_result_t The first element in decode result is the
   *                              terminal state of the search result. The
   *                              second element (optional) is the terminal
   *                              state of the gold transition sequence.
   */
  const_decode_result_t decode(const _StateType& initial_state,
      const action_sequence_t& gold_actions,
      int max_nr_actions) {
    floatval_t max_violate_diff = 0;
    const _StateType* max_violate_predite_state = NULL;
    const _StateType* max_violate_correct_state = NULL;

    //use_avg = (gold_actions.size() == 0);
    _StateType* row = allocate_lattice(0);
    const _StateType* correct_state = NULL;

    // set the initial state.
    row[0].copy(initial_state);
    // set the lattice size to zero.
    lattice_size.clear();
    lattice_size.push_back(1);
    correct_state = row;

    for (step = 1; step <= max_nr_actions; ++ step) {
      //_TRACE << "sys: round " << step;

      row = allocate_lattice(step);
      lattice_size.push_back(0);
      int& current_beam_size = lattice_size[step];

      for (int i = 0; i < lattice_size[step- 1]; ++ i) {
        _StateType* source = lattice_heads[step- 1]+ i;
        get_possible_actions((*source));
        score_possible_actions((*source));

        for (int i = 0; i < possible_actions.size(); ++ i) {
          const _ActionType& act = possible_actions[i];
          current_beam_size += extend_candidate_transition(
              scored_transition_t(source, act, source->score+ packed_scores[act]),
              current_beam_size);
        }
      }

      for (int i = 0; i < current_beam_size; ++ i) {
        const scored_transition_t& trans = candidate_transitions[i];
        transit((*trans.template get<0>()), trans.template get<1>(), trans.template get<2>(),
            (row+ i));
      }

      if (gold_actions.size() != 0) {
        // Perform training and early update.
        const _StateType* next_correct_state = search_correct_state(
            gold_actions[step- 1], correct_state,
            row, row + current_beam_size);

        const _StateType* best_target = search_best_state(
            row, row + current_beam_size);

        bool dropout = false;

        if (NULL == next_correct_state) {
          //_TRACE << "sys: error at step =" << step;
          _StateType* dummy_state = row+ beam_size;
          _ScoreContextType ctx(*correct_state);
          transit((*correct_state), gold_actions[step- 1],
              model->score(ctx, gold_actions[step- 1], use_avg), dummy_state);
          correct_state = dummy_state;
          dropout = true;
        } else {
          correct_state = next_correct_state;
        }

        if (dropout && update_strategy == kEarlyUpdate) {
          _TRACE << "sys: decode is done (cond: early update).";
          return const_decode_result_t(best_target, correct_state);
        }

        if (update_strategy == kMaxViolation) {
          floatval_t mdiff = best_target->score - correct_state->score;
          if (max_violate_predite_state == NULL || max_violate_diff <= mdiff) {
            _TRACE << "sys: violate update " << mdiff;
            max_violate_diff = mdiff;
            max_violate_predite_state = best_target;
            max_violate_correct_state = correct_state;
          }
        }
      }
    }

    _TRACE << "sys: decode is done (cond 2).";
    if (update_strategy == kMaxViolation) {
      return const_decode_result_t(max_violate_predite_state, max_violate_correct_state);
    } else {
      -- step;
      const _StateType* best_target = search_best_state(row, row + lattice_size[step]);
      return const_decode_result_t(best_target, correct_state);
    }
  }

  void reset_beam_size(int size) {
    if (beam_size < size) {
      for (std::size_t i = 0; i < lattice_heads.size(); ++ i) {
        _StateType* p = new _StateType[size+ 1];
        std::copy(lattice_heads[i], lattice_heads[i]+ beam_size + 1, p);
        delete [](lattice_heads[i]);
        lattice_heads[i] = p;
      }
      delete [](candidate_transitions);
      candidate_transitions = new scored_transition_t[size];
    }
    beam_size = size;
  }
private:
  /**
   * allocate the lattice of index-th row.
   *
   *  @param[in]  index   The row index.
   */
  _StateType* allocate_lattice(int index) {
    if (index >= lattice_heads.size()) {
      for (size_t i = lattice_heads.size(); i <= index; ++ i) {
        _StateType* lattice_head = new _StateType[beam_size+ 1];
        lattice_heads.push_back(lattice_head);
        // Allocate one more space to the dummy state.
      }
    }
    return lattice_heads.at(index);
  }
protected:
  /**
   * In class function for invoking the get possible action funtions. It's used
   * by the decode funtion.
   *
   *  @param[in]  source  The source state.
   */
  void get_possible_actions(const _StateType& source) {
    get_possible_actions(source, possible_actions);
  }

  /**
   * Get the possible transition actions for the source state.
   *
   *  @param[in]  source    The source state.
   *  @param[out] actions   The output possible actions.
   */
  virtual void get_possible_actions(const _StateType& source,
      action_collection_t& actions) = 0;

  /**
   * In class function for invoking the score possible actions functions. It's
   * used by the decode function.
   *
   *  @param[in]  source    The source state.
   */
  void score_possible_actions(const _StateType& source) {
    score_possible_actions(source, possible_actions, packed_scores);
  }

  /**
   * Calculate the score transit from the source state and store the result
   * in the scores.
   *
   *  @param[in]  source  The source state.
   *  @param[in]  actions The possible actions.
   *  @param[out] scores  The score cache.
   */
  virtual void score_possible_actions(const _StateType& source,
      const action_collection_t& actions,
      packed_score_t& scores) {
    for (typename packed_score_t::iterator itx = scores.begin();
        itx != scores.end(); ++ itx) {
      itx->second = 0;
    }
    //scores.clear();
    _ScoreContextType ctx(source);
    model->batchly_score(ctx, actions, use_avg, scores);
  }

  /**
   * Insert the ScoredTransition into beam.
   *
   *  @param[in]  trans               The scored transition.
   *  @param[in]  current_beam_size   The current beam size.
   *  @return     int                 The number of inserted transition.
   */
  int extend_candidate_transition(const scored_transition_t& trans,
      int current_beam_size) {
    if (current_beam_size == beam_size) {
      if (trans.template get<2>() > candidate_transitions[0].template get<2>()) {
        std::pop_heap(candidate_transitions,
            candidate_transitions + beam_size,
            ScoredTransitionMore);
        candidate_transitions[beam_size- 1] = trans;
        std::push_heap(candidate_transitions,
            candidate_transitions + beam_size,
            ScoredTransitionMore);
      }
      return 0;
    }
    candidate_transitions[current_beam_size] = trans;
    std::push_heap(candidate_transitions,
        candidate_transitions + current_beam_size + 1,
        ScoredTransitionMore);
    return 1;
  }

  /**
   * Perform transition action act on the state stored in from and result in
   * the state to.
   *
   *  @param[in]  source  The fron state.
   *  @param[in]  act     The action.
   *  @param[in]  score   The pre-calculated score of this transition.
   *  @param[out] target  The output state.
   */
  virtual void transit(const _StateType& source, const _ActionType& act,
      const floatval_t& score, _StateType* target) = 0;

  /**
   * Search the correct state from certain rang of states.
   *
   *  @param[in] act                    The action.
   *  @param[in] previous_correct_state The correct state.
   *  @param[in] begin                  The starting of the states.
   *  @param[in] end                    The ending of the states.
   *  @return    const StateItem*       The pointer to the state.
   */
  const _StateType* search_correct_state(const _ActionType& act,
      const _StateType* previous_correct_state,
      const _StateType* begin,
      const _StateType* end) {
    const _StateType* correct_state = NULL;
    for (const _StateType* p = begin; p != end; ++ p) {
      //_TRACE << (void *)p->previous << " -> " << (void *)p << " by " << p->last_action;
      if (p->previous == previous_correct_state && p->last_action == act) {
        correct_state = p;
        break;
      }
    }
    return correct_state;
  }

  /**
   * Search the best state in the given range.
   *
   *  @param[in]  begin             The begining of this range.
   *  @param[in]  end               The end of this range.
   *  @return     const StateItem*  The pointer to the best state.
   */
  const _StateType* search_best_state(const _StateType* begin,
      const _StateType* end) {
    const _StateType* best = begin;
    for (const _StateType* p = begin+ 1; p != end; ++ p) {
      if (p->score > best->score) {
        best = p;
      }
    }
    return best;
  }
};

} //  end for namespace ZuoPar

#endif  //  end for __ZUOPAR_SYSTEM_SYSTEM_H__

