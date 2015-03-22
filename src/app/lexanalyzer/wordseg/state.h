#ifndef __ZUOPAR_APP_LEXANALYZER_WORDSEG_STATE_H__
#define __ZUOPAR_APP_LEXANALYZER_WORDSEG_STATE_H__

#include "types/common.h"
#include "types/segmentation.h"
#include "app/settings.h"
#include "app/lexanalyzer/wordseg/action.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

class State {
public:
  //! The empty constructor.
  State();

  /**
   * The constructor for the State.
   *
   *  @param[in]  r   The pointer to the dependency state.
   */
  State(const Segmentation* r);

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
  bool split(const State& source);

  /**
   * Perform the shift action from source state.
   *
   *  @param[in]  source  The source state.
   *  @param[in]  postag  The postag.
   */
  bool append(const State& source);

  bool terminated() const;    //! Shorthand for buffer == ref->size()

  int index;                  //! The transition works between ith and (i+1)th char.
  int current_word_index;     //! The begining index of the current word.
  int previous_word_index;    //! The begining index of the previous word.
  const Segmentation* ref;    //! The pointer to the dependency tree.
  const State* previous;      //! The pointer to the previous state
  floatval_t score;           //! The score.
  Action last_action;         //! The last action.
};

} //  end for namespace chinesewordsegmentor
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
#endif  //  end for __ZUOPAR_APP_LEXANALYZER_WORDSEG_STATE_H__
