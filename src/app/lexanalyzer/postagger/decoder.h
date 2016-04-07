#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_DECODER_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_DECODER_H__

#include "engine/token_alphabet.h"
#include "system/system.h"
#include "app/lexanalyzer/postagger/state.h"
#include "app/lexanalyzer/postagger/action.h"
#include "app/lexanalyzer/postagger/score_context.h"
#include "app/lexanalyzer/postagger/weight.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

namespace eg = ZuoPar::Engine;

class Decoder: public TransitionSystem<Action, State, Weight> {
public:
  /**
   * The decoder constructor.
   *
   *  @param[in]  n         The number of dependency relations.
   *  @param[in]  beam      The size of beam.
   *  @param[in]  avg       Use average parameter?
   *  @param[in]  strategy  The update stategy (for learning).
   *  @param[in]  weight    The pointer to the weight.
   */
  Decoder(int n, int beam, bool avg, UpdateStrategy strategy, Weight* weight);

  //! Implement arc standard get possible actions.
  void get_possible_actions(const State& source,
      std::vector<Action>& actions);

  //! Implement arc standard transition.
  void transit(const State& source, const Action& act, const floatval_t& score,
      State* target);

  bool terminated();
private:
  int n_postags;
};

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar


#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_DECODER_H__
