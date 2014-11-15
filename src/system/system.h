#ifndef __ZUOPAR_SYSTEM_SYSTEM_H__
#define __ZUOPAR_SYSTEM_SYSTEM_H__

#include <iostream>
#include <vector>
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

private:
  //!
  const _ModelType& model;
  //!
  lattice_head_list_t lattice_heads;
  //!
  lattice_size_list_t lattice_size;
  //!
  int beam_size;

protected:
  //!
  action_collection_t possible_actions;
  //!
  packed_score_t packed_scores;

public:
  TransitionSystem(
      int _beam_size,
      const _ModelType& _model)
    : beam_size(_beam_size),
    model(_model) {
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
   *  @param[in]  initial_state The initial state.
   *  @param[in]  gold_actions  The gold actions.
   */
  decode_result_t decode(const _StateType& initial_state,
      const action_sequence_t& gold_actions,
      int max_nr_actions) {
    _StateType* row = allocate_lattice(0);
    row[0].copy(initial_state);

    int step = 1;
    for (step = 1; step < max_nr_actions; ++ step) {
      _TRACE << "decode: round " << step;
      int current_beam_size = 0;
      for (int i = 0; i < lattice_size[step- 1]; ++ i) {
        _StateType* source = lattice_heads[step- 1]+ i;
        _TRACE << "decode: extend from state " << (void *)source;
        get_possible_actions((*source));
      }
    }
  }

private:
  /**
   * allocate the lattice of index-th row.
   *
   *  @param[in]  index   The row index.
   */
  _StateType* allocate_lattice(int index) {
    if (index > lattice_heads.size()) {
      for (size_t i = lattice_heads.size(); i <= index; ++ i) {
        lattice_heads.push_back(NULL);
        lattice_heads.back() = new _StateType[beam_size];
      }
    }
    return lattice_heads[index];
  }
protected:
  /**
   *
   *
   *
   */
  virtual void get_possible_actions(const _StateType& item) = 0;

  /**
   * Perform transition action act on the state stored in from and result in
   * the state to.
   *
   *  @param[in]  from  The fron state.
   *  @param[in]  act   The action.
   *  @param[in]  score The pre-calculated score of this transition.
   *  @param[out] to    The output state
   */
  virtual void transit(const _StateType& source, const _ActionType& act,
      floatval_t score, _StateType* target) {
  }

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

