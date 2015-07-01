#ifndef __ZUOPAR_APP_LEXANALYZER_WORDSEG_DECODER_H__
#define __ZUOPAR_APP_LEXANALYZER_WORDSEG_DECODER_H__

#include "system/system.h"
#include "app/lexanalyzer/wordseg/state.h"
#include "app/lexanalyzer/wordseg/action.h"
#include "app/lexanalyzer/wordseg/score_context.h"
#include "app/lexanalyzer/wordseg/weight.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

class Decoder: public TransitionSystem<Action, State, Weight> {
public:
  /**
   * The decoder constructor.
   *
   *  @param[in]  nr        The number of dependency relations.
   *  @param[in]  beam_size The size of beam.
   *  @param[in]  weight    The pointer to the weight.
   */
  Decoder(int beam_size, bool avg, Weight* weight);

  //! Implement arc standard get possible actions.
  void get_possible_actions(const State& source,
      std::vector<Action>& actions);

  //! Implement arc standard transition.
  void transit(const State& source, const Action& act, const floatval_t& score,
      State* target);

  bool terminated();
};

} //  end for namespace chinesewordsegmentor
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar


#endif  //  end for __ZUOPAR_APP_LEXANALYZER_WORDSEG_DECODER_H__
