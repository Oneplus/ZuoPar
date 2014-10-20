#ifndef __ZUOPAR_SYSTEM_SYSTEM_H__
#define __ZUOPAR_SYSTEM_SYSTEM_H__

#include <iostream>
#include <vector>
#include "settings.h"
#include "boost/unordered_map.hpp"
#include "model/model.h"

namespace ZuoPar {

template <class ActionType,
          class StateType,
          class ScoreContextType,
          class InputType>
class TransitionSystem {
public:
  //! Define the type of const decode result.
  typedef std::pair<const StateType*, const StateType*> const_decode_result_t;
  //! Define the type of regular decode result.
  typedef std::pair<StateType*, StateType*> decode_result_t;

private:
  //! Define a sequence of actions.
  typedef std::vector<ActionType> action_sequence_t;
  //! Define a set of actions.
  typedef std::vector<ActionType> action_collection_t;
  //! Define the mapping between action and it's packed score.
  typedef boost::unordered_map<ActionType, floatval_t> packed_score_t;
  //! Define the (source.state, action, score) tuple.
  typedef boost::tuples::tuple<const StateType*, ActionType, floatval_t> scored_transition_t;

private:
  //!
  const Model<StateType, ScoreContextType, ActionType>& model;
  //!
  StateType** lattice_index;
  //!
  int* lattice_size;

  //!
  int max_nr_actions;
  //!
  int beam_size;
public:
  TransitionSystem(
      int _max_nr_of_actions,
      int _beam_size,
      const Model<StateType, ScoreContextType, ActionType>& model) 
    : max_nr_of_actions(_max_nr_of_actions),
    beam_size(_beam_size)  {
    lattice_index = new StateType*[max_number_of_actions];
    lattice_size = new int[max_number_of_actions];

    for (int i = 0; i < max_number_of_actions; ++ i) {
      lattice_index[i] = 0;
      lattice_size[i] = 0;
    }
  }

  ~TransitionSystem() {
    for (int i = 0; i < max_nr_actions; ++ i) {
      if (lattice_index[i]) {
        delete [](lattice_index[i]);
        lattice_index[i] = 0;
      }
    }

  }

  /**
   *
   *
   *
   *
   */
  decode_result_t decode(const InputType& input,
      const action_sequence_t& gold_actions) {
  }

protected:
  /**
   *
   *
   *
   */
  virtual void before_decoding(const InputType& input,
      const action_sequence_t& gold_actions) = 0;

  /**
   *
   *
   */
  virtual void after_decoding(const InputType& input,
      const action_sequence_t& gold_actions) = 0;

  /**
   *
   *
   *
   */
  virtual void get_possible_actions(const StateType& item) = 0;

  /**
   * Perform transition action act on the state stored in from and result in
   * the state to.
   *
   *  @param[in]  from  The fron state.
   *  @param[in]  act   The action.
   *  @param[in]  score The pre-calculated score of this transition.
   *  @param[out] to    The output state
   */
  void transit(const StateType& source, const ActionType& act,
      floatval_t score, StateType* target);

  /**
   *
   *
   *  @param[in] act                    The action.
   *  @param[in] previous_correct_state The correct state.
   *  @param[in/out]  begin             The starting of the states.
   *  @param[in/out]  end               The ending of the states.
   *  @return         const StateItem*  The pointer to the state.
   */
  const StateType* search_correct_state(const ActionType& act,
      const StateType* previous_correct_state,
      StateType* begin, StateType* end);

  /**
   * Search the best state in the given range.
   *
   *  @param[in]  begin   The begining of this range.
   *  @param[in]  end     The end of this range.
   *  @return     const StateItem*  The pointer to the best state.
   */
  const StateType* search_best_state(const StateType* begin,
      const StateType* end);
};

} //  end for namespace ZuoPar

#endif  //  end for __ZUOPAR_SYSTEM_SYSTEM_H__

