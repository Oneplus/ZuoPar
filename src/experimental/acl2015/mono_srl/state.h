#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_STATE_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_STATE_H__

#include "types/common.h"
#include "types/semchunks.h"
#include "experimental/acl2015/mono_srl/action.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoSRL {

class State {
public:
  //! The empty constructor.
  State();

  /**
   * The constructor for the State.
   *
   *  @param[in]  r   The pointer to the dependency state.
   */
  State(const MonoSemanticChunks* r);

  /**
   * This method is needed by @class TransitionSystem.
   *
   *  @param[in]  source  The source of state to copy from.
   */
  void copy(const State& source);

  //! Clear the state.
  void clear();

  //! Perform the O action from source state.
  bool O(const State& source);

  /**
   * Perform the shift action from source state.
   *
   *  @param[in]  source  The source state.
   */
  bool B(const State& source, const tag_t& tag);

  /**
   * Perform the shift action from source state.
   *
   *  @param[in]  source  The source state.
   */
  bool I(const State& source, const tag_t& tag);

  //! If the state reach the terminating state.
  bool terminated() const;

  //! The front word in the buffer.
  int buffer;

  //! The pointer to the previous state.
  const State* previous;

  //! The pointer to the dependency tree.
  const MonoSemanticChunks* ref;

  //! The score.
  floatval_t score;

  //! The last action.
  Action last_action;
};

} //  namespace monosrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_STATE_H__
