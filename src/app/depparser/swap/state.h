#ifndef __ZUOPAR_APP_DEPPARSER_SWAP_STATE_H__
#define __ZUOPAR_APP_DEPPARSER_SWAP_STATE_H__

#include "types/common.h"
#include "types/dependency.h"
#include "app/settings.h"
#include "app/depparser/swap/action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

class State {
public:
  //! The empty constructor.
  State();

  /**
   * The constructor for the State.
   *
   *  @param[in]  r   The pointer to the dependency state.
   */
  State(const CoNLLXDependency* r);

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
   * Perform  the swap from source state onto current state.
   *
   *  @param[in]  source  The source state.
   */
  bool swap(const State& source);

  bool buffer_empty() const;  //! shorthand for buffer.empty()
  bool is_complete() const;   //! shorthand for stack.size() == 1 && buffer.empty()
  size_t stack_size() const;  //! shorthand for stack.size()

  //! Refresh the value of top0 and top1.
  void _update_stack_information();

  //! Refresh left children information
  void _update_left_children_information(int h, int m);

  //! Refresh right children information
  void _update_right_children_information(int h, int m);

  //! Update the left label set.
  void _update_left_label_set(int h, int deprel);

  //! Update the right label set.
  void _update_right_label_set(int h, int deprel);

  //! The pointer to the previous state.
  std::vector<int> stack;

  //! The front word in the buffer.
  std::vector<int> buffer;

  //! The pointer to the previous state.
  const State* previous;

  //! The pointer to the dependency tree.
  const CoNLLXDependency* ref;

  //! The score.
  floatval_t score;

  //! The last action.
  Action last_action;

  //! The top word on the stack.
  int top0;
  //! The second top word on the stack.
  int top1;
  //! The front word on the buffer.
  int front0;
  //! The second word on the buffer.
  int front1;

  //! Use to record the heads in current state.
  int heads[kMaxNumberOfWords];

  //! The dependency relation cached in state.
  deprel_t deprels[kMaxNumberOfWords];

  //! The left label set.
  unsigned left_label_set_lowbit[kMaxNumberOfWords];
  unsigned left_label_set_highbit[kMaxNumberOfWords];

  //! The right label set.
  unsigned right_label_set_lowbit[kMaxNumberOfWords];
  unsigned right_label_set_highbit[kMaxNumberOfWords];

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

} //  end for namespace swap
} //  end for namespace dependencyparser
} //  end for namespace zuopar
#endif  //  end for __ZUOPAR_APP_DEPPARSER_SWAP_STATE_H__
