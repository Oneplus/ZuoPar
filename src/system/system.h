#ifndef __ZUOPAR_SYSTEM_SYSTEM_H__
#define __ZUOPAR_SYSTEM_SYSTEM_H__

#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/unordered_map.hpp>
#include "settings.h"
#include "utils/logging.h"

namespace ZuoPar {

template <class _ActionType,
          class _StateType,
          class _ScoreContextType,
          class _ModelType>
class TransitionSystem {
public:
  //! Define the type of const decode result.
  typedef std::pair<const _StateType*, const _StateType*> const_decode_result_t;
  //! Define the type of regular decode result.
  typedef std::pair<_StateType*, _StateType*> decode_result_t;
  //! Define a sequence of actions.
  typedef std::vector<_ActionType> action_sequence_t;

private:
  //! Define a set of actions.
  typedef std::vector<_ActionType>  action_collection_t;

  //!
  typedef std::vector<_StateType*>  lattice_head_list_t;

  //!
  typedef std::vector<int>          lattice_size_list_t;

  //! Define the mapping between action and it's packed score.
  typedef boost::unordered_map<_ActionType, floatval_t> packed_score_t;

  //! Define the (source.state, action, score) tuple.
  typedef boost::tuples::tuple<const _StateType*, _ActionType, floatval_t> scored_transition_t;

  //! The comparsion function between two scored transition.
  static bool ScoredTransitionMore(const scored_transition_t& x,
      const scored_transition_t& y) {
    return x.template get<2>() > y.template get<2>();
  }
private:
  //!
  _ModelType* model;
  //!
  lattice_head_list_t lattice_heads;
  //!
  lattice_size_list_t lattice_size;

  //! The size of the beam.
  int beam_size;

  //! Use to specify if use averaged parameter.
  bool use_avg;

  //!
  _StateType dummy_state;
protected:
  //!
  action_collection_t possible_actions;

  //!
  packed_score_t packed_scores;

  //!
  scored_transition_t* candidate_transitions;
public:
  TransitionSystem(
      int _beam_size,
      _ModelType* _model)
    : beam_size(_beam_size),
    model(_model) {
    candidate_transitions = new scored_transition_t[beam_size];
  }

  ~TransitionSystem() {
    for (size_t i = 0; i < lattice_heads.size(); ++ i) {
      if (lattice_heads[i]) {
        delete [](lattice_heads[i]);
        lattice_heads[i] = 0;
      }
    }
  }

  /**
   * The main process of decoding.
   *
   *  @param[in]  initial_state   The initial state.
   *  @param[in]  gold_actions    The gold actions.
   *  @return     decode_result_t The first element in decode result is the
   *                              terminal state of the search result. The
   *                              second element (optional) is the terminal
   *                              state of the gold transition sequence.
   */
  const_decode_result_t decode(const _StateType& initial_state,
      const action_sequence_t& gold_actions,
      int max_nr_actions) {
    use_avg = (gold_actions.size() == 0);
    _StateType* row = allocate_lattice(0);
    const _StateType* correct_state = NULL;
    lattice_size.clear();

    // set the initial state.
    row[0].copy(initial_state);
    // set the lattice size to zero.
    lattice_size.push_back(1);
    correct_state = row;

    int step = 1;
    for (step = 1; step < max_nr_actions; ++ step) {
      _TRACE << "sys: round " << step;

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

      _TRACE << "sys: nr extended states(current beam size) =" << current_beam_size;
      for (int i = 0; i < current_beam_size; ++ i) {
        const scored_transition_t& trans = candidate_transitions[i];
        _TRACE << "sys: trans from " << ((void *)trans.template get<0>())
          << " into " << (void *)(row+ i) << " by " << trans.template get<1>()
          << " " << trans.template get<2>();
        transit(*trans.template get<0>(),
            trans.template get<1>(),
            trans.template get<2>(),
            (row+ i));
      }

      if (gold_actions.size() != 0) {
        // Perform training and early update.
        const _StateType* next_correct_state = search_correct_state(
            gold_actions[step- 1], correct_state,
            row, row + current_beam_size);

        bool correct_state_in_beam = (next_correct_state != NULL);
        if (!correct_state_in_beam) {
          _TRACE << "decode: error at step =" << step;
          const _StateType* best_target = search_best_state(
              row, row + current_beam_size);
          transit((*correct_state), gold_actions[step- 1], 0, &dummy_state);
          return const_decode_result_t(best_target, &dummy_state);
        } else {
          correct_state = next_correct_state;
        }
      }
    }
    const _StateType* best_target = search_best_state(row, row + lattice_size[step]);
    return const_decode_result_t(best_target, correct_state);
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
        lattice_heads.push_back(NULL);
        lattice_heads.back() = new _StateType[beam_size];
      }
    }
    return lattice_heads.at(index);
  }
protected:
  /**
   *
   *
   *
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
   *
   *
   *
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
  void score_possible_actions(const _StateType& source,
      const action_collection_t& actions,
      packed_score_t& scores) {
    scores.clear();
    _ScoreContextType ctx(source);
    for (int i = 0; i < actions.size(); ++ i) {
      const _ActionType& act = actions[i];
      scores[act] = model->score(ctx, act, use_avg);
    }
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
   *  @param[in/out]  begin             The starting of the states.
   *  @param[in/out]  end               The ending of the states.
   *  @return         const StateItem*  The pointer to the state.
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
   *  @param[in]  begin   The begining of this range.
   *  @param[in]  end     The end of this range.
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

