#ifndef __ZUOPAR_APP_SEMCHUNKER_MONO_STATE_H__
#define __ZUOPAR_APP_SEMCHUNKER_MONO_STATE_H__

#include "types/common.h"
#include "types/semchunks.h"
#include "app/semchunker/mono/information.h"
#include "app/semchunker/mono/action.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

class State {
public:
  //! The empty constructor.
  State();

  /**
   * The constructor for the State.
   *
   *  @param[in]  ref_  The pointer to the monosemantic chunk instance.
   *  @param[in]  info_ The pointer to the information.
   */
  State(const MonoSemanticChunks* ref_, const Information* info_);

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

  //! The path.
  const Information* info;

  //! The pointer to the dependency tree.
  const MonoSemanticChunks* ref;

  //! The score.
  floatval_t score;

  //! The last action.
  Action last_action;

  //! The low bit for label set.
  int label_set;
};

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEMCHUNKER_MONO_STATE_H__
