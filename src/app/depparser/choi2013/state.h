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

  bool buffer_empty() const;          //! shorthand for buffer == ref->size()
  bool stack_empty() const;           //! shorthand for stack.empty()
  bool reach_last_token() const;      //! shorthand for buffer + 1 == ref->size()
  bool stack_top_has_head() const;    //! shorthand for heads[top0] != -1.
  bool buffer_front_has_head() const; //! shorthand for heads[buffer] != -1.
  bool is_complete() const;           //! shorthand for stack.size() == 1 &&
                                      //! buffer == ref->size()
  size_t stack_size() const;          //! shorthand for stack.size()

  //! Is descendant?
  bool is_descendant(int grandparent, int child) const;

  const State* previous;  //! The pointer to the previous state.
  floatval_t score;       //! The score.
  Action last_action;     //! The last action.
  int top0;               //! The top0 element.
  int top1;               //! The top1 element.

  void _shift();    //! auxiliary function to perform *-SHIFT action.
  void _reduce();   //! auxiliary function to perform *-REDUCE action.
  void _pass();     //! auxiliary function to perform *-PASS action.

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

  //! The stack.
  std::vector<int> stack;

  //! The deque
  std::vector<int> deque;

  //! The front word in the buffer.
  int buffer;

  //! The pointer to the dependency tree.
  const CoNLLXDependency* ref;

  //! The number of empty heads in the stack.
  int nr_empty_heads;

  //! Use to record the heads in current state.
  int heads[kMaxNumberOfWords];

  //! The dependency relations.
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

} //  end for namespace choi2013
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCEAGER_STATE_H__
