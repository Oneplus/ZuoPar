#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_DECODER_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_DECODER_H__

#include "engine/token_alphabet.h"
#include "system/system.h"
#include "app/depparser/arcstandard/state.h"
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/action_utils.h"
#include "app/depparser/arcstandard/score_context.h"
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/weight.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

namespace eg = ZuoPar::Engine;

class Decoder: public TransitionSystem<Action, State, ScoreContext, Weight> {
public:
  Decoder(int nr, int beam_size, Weight* weight)
    : nr_deprels(nr),
    TransitionSystem<Action, State, ScoreContext, Weight>(beam_size, weight) {
  }

  //! Implement arc standard get possible actions.
  void get_possible_actions(const State& source,
      std::vector<Action>& actions) {
    actions.clear();
    if (!source.buffer_empty()) {
      actions.push_back(ActionFactory::make_shift());
    }

    if (source.stack_size() >= 2) {
      //
      for (deprel_t l = eg::TokenAlphabet::END+ 1; l < nr_deprels; ++ l) {
        actions.push_back(ActionFactory::make_left_arc(l));
      }
      for (deprel_t l = eg::TokenAlphabet::END+ 1; l < nr_deprels; ++ l) {
        actions.push_back(ActionFactory::make_right_arc(l));
      }
    }
  }

  //! Implement arc standard transition.
  void transit(const State& source, const Action& act, const floatval_t& score,
      State* target) {
    int deprel;
    if (ActionUtils::is_shift(act)) {
      target->shift(source);
    } else if (ActionUtils::is_left_arc(act, deprel)) {
      target->left_arc(source, deprel);
    } else if (ActionUtils::is_right_arc(act, deprel)) {
      target->right_arc(source, deprel);
    }
  }

private:
  int nr_deprels;
};

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar


#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_DECODER_H__
