#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_DECODER_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_DECODER_H__

#include "engine/token_alphabet.h"
#include "system/system.h"
#include "app/depparser/arcstandard/state.h"
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/score_context.h"
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/weight.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

namespace eg = ZuoPar::Engine;

class Decoder: public TransitionSystem<Action, State, ScoreContext, Weight> {
public:
  Decoder(int nr, int beam_size, Weight* weight);

  //! Implement arc standard get possible actions.
  void get_possible_actions(const State& source,
      std::vector<Action>& actions);

  //! Implement arc standard transition.
  void transit(const State& source, const Action& act, const floatval_t& score,
      State* target);

private:
  int nr_deprels;
};

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar


#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_DECODER_H__
