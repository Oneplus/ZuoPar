#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_STATE_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_STATE_H__

#include "types/common.h"
#include "types/semchunks.h"
#include "experimental/acl2015/multi_predicate_srl1/information.h"
#include "experimental/acl2015/multi_predicate_srl1/action.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

class State {
public:
  //! The empty constructor.
  State();

  /**
   * The constructor for the State.
   *
   *  @param[in]  r   The pointer to the dependency state.
   */
  State(const SemanticChunks& r, const Information& i);

  /**
   * This method is needed by @class TransitionSystem.
   *
   *  @param[in]  source  The source of state to copy from.
   */
  void copy(const State& source);

  //! Clear the state.
  void clear();

  //! Perform the transition.
  bool take(const State& source, const ActionCollection& collection);

  //! If the state reach the terminating state.
  bool terminated() const;

  //! The front word in the buffer.
  int buffer;

  //! The pointer to the previous state.
  const State* previous;

  //! The pointer to the dependency tree.
  const SemanticChunks* ref;

  //! The information cache for the state.
  const Information* info;

  //! The score.
  floatval_t score;

  //! The label set.
  //std::vector<int> label_set;

  //! The last action.
  ActionCollection last_action;
};

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_STATE_H__
