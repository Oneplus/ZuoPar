#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_STATE_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_STATE_H__

#include "types/common.h"
#include "types/sequence_instance.h"
#include "app/settings.h"
#include "app/seqlabeler/trans/action.h"

namespace ZuoPar {
namespace SequenceLabeler {

class State {
public:
  //! The empty constructor.
  State();

  /**
   * The constructor for the State.
   *
   *  @param[in]  r   The pointer to the sequence_instance state.
   */
  State(const SequenceInstance* r);

  /**
   * This method is needed by @class TransitionSystem.
   *
   *  @param[in]  source  The source of state to copy from.
   */
  void copy(const State& source);

  //! Clear the state.
  void clear();

  /**
   * Perform the shift action from source state.
   *
   *  @param[in]  source  The source state.
   *  @param[in]  tag     The tag.
   */
  bool tag(const State& source, tag_t tag);

  //! If the state reach the terminating state.
  bool terminated() const;

  tag_t tags[kMaxNumberOfWords];  //! The cached tags.
  int buffer;                   //! The front word in the buffer.
  const State* previous;        //! The pointer to the previous state.
  const SequenceInstance* ref;  //! The pointer to the sequence instance tree.

  //! The score.
  floatval_t score;

  //! The last action.
  Action last_action;
};

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_STATE_H__
