#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_STATE_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_STATE_H__

#include "types/common.h"
#include "types/postag.h"
#include "app/settings.h"
#include "app/lexanalyzer/postagger/action.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

class State {
public:
  //! The empty constructor.
  State();

  /**
   * The constructor for the State.
   *
   *  @param[in]  r   The pointer to the dependency state.
   */
  State(const PostagWithLiteralCache* r);

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
   *  @param[in]  postag  The postag.
   */
  bool tag(const State& source, postag_t postag);

  //! If the state reach the terminating state.
  bool terminated() const;

  //! The cached postags. Actually, this attributes is not needed because the
  //! postag sequence can be recovered from decoding matrix.
  postag_t postags[kMaxNumberOfWords];

  //! The front word in the buffer.
  int buffer;

  //! The pointer to the previous state.
  const State* previous;

  //! The pointer to the dependency tree.
  const PostagWithLiteralCache* ref;

  //! The score.
  floatval_t score;

  //! The last action.
  Action last_action;
};

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_STATE_H__
