#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_DECODER_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_DECODER_H__

#include "system/system.h"
#include "app/depparser/arcstandard/state.h"
#include "app/depparser/arcstandard/score_context.h"
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/weight.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

class Decoder: public TransitionSystem<Action, State, ScoreContext, Weight> {
public:
  Decoder(int beam_size, Weight& weight)
    : TransitionSystem<Action, State, ScoreContext, Weight>(beam_size, weight) {
  }

  void get_possible_actions(const State& item) {
  }
};

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar


#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_DECODER_H__
