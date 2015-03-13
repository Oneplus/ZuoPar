#ifndef __ZUOPAR_APP_DEPPARSER_CHOI2013_STATE_H__
#define __ZUOPAR_APP_DEPPARSER_CHOI2013_STATE_H__

#include "types/common.h"
#include "types/dependency.h"
#include "app/settings.h"
#include "app/depparser/choi2013/action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

class State {
public:
  //! empty constructor.
  State();

  /**
   * The constructor for the State.
   *
   *  @param[in]  r   The pointer to the dependency state.
   */
  State(const Dependency* r);

  /**
   * This method is needed by @class TransitionSystem.
   *
   *  @param[in]  source  The source of state to copy from.
   */
  void copy(const State& source);

  //! Clear the state.
  void clear();

  /**
   * Perform the IDLE action from source state.
   *
   *  @param[in]  source  The source state.
   */
  bool idle(const State& source);

  /**
   * Perform the shift action from source state.
   *
   *  @param[in]  source  The source state.
   */
  bool shift(const State& source);

  /**
   * Perform the reduce action from source state.
   *
   *  @param[in]  source  The source state.
   */
  bool reduce(const State& source);

  /**
   * Perform the no pass action from source state.
   *
   *  @param[in]  source  The source state.
   */
  bool no_pass(const State& source);

  /**
   * Perform the left arc action from source state onto current state.
   *
   *  @param[in]  source  The source state.
   *  @param[in]  deprel  The dependency relation.
   */
  bool left_arc(const State& source, deprel_t deprel);

  /**
   * Perform the right arc action from source state onto current state.
   *
   *  @param[in]  source  The source state.
   *  @param[in]  deprel  The dependency relation.
   */
  bool right_arc(const State& source, deprel_t deprel);

  /**
   * Perform the left pass action from source state onto current state.
   *
   *  @param[in]  source  The source state.
   *  @param[in]  deprel  The dependency relation.
   */
  bool left_pass(const State& source, deprel_t deprel);

  /**
   * Perform the right pass action from source state onto current state.
   *
   *  @param[in]  source  The source state.
   *  @param[in]  deprel  The dependency relation.
   */
  bool right_pass(const State& source, deprel_t deprel);

  void _shift();
  void _reduce();
  void _pass();
  //! Return true on the buffer is empty.
  bool buffer_empty() const;
  bool stack_empty() const;
  bool reach_last_token() const;

  bool stack_top_has_head() const;

  bool buffer_front_has_head() const;

  bool is_descendant(int grandparent, int child) const;

  //! Return this state reach the final state.
  bool is_complete() const;

  //! Get the size of the stack.
  size_t stack_size() const;

  //! Refresh the value of top0 and top1.
  void refresh_stack_information();

  //! The stack.
  std::vector<int> stack;

  //! The deque
  std::vector<int> deque;

  //! The front word in the buffer.
  int buffer;

  //! The pointer to the previous state.
  const State* previous;

  //! The pointer to the dependency tree.
  const Dependency* ref;

  //! The score.
  floatval_t score;

  //! The last action.
  Action last_action;

  //! The top0 element.
  int top0;

  int nr_empty_heads;

  //! Use to record the heads in current state.
  int heads[kMaxNumberOfWords];

  //!
  deprel_t deprels[kMaxNumberOfWords];

  //!
  int left_label_set[kMaxNumberOfWords];

  //!
  int right_label_set[kMaxNumberOfWords];

  //! Use to record the number of left children in current state.
  int nr_left_children[kMaxNumberOfWords];

  //! Use to record the number of right children in current state.
  int nr_right_children[kMaxNumberOfWords];

  //! Use to record the left most child for each word in current state.
  int left_most_child[kMaxNumberOfWords];

  //! Use to record the right most child for each word in current state.
  int right_most_child[kMaxNumberOfWords];

  //! Use to record the left 2nd-most child for each word in current state.
  int left_2nd_most_child[kMaxNumberOfWords];

  //! Use to record the right 2nd-most child for each word in current state.
  int right_2nd_most_child[kMaxNumberOfWords];
};

} //  end for namespace choi2013
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCEAGER_STATE_H__
