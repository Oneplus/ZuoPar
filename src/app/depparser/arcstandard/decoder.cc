#include "app/depparser/arcstandard/decoder.h"
#include "app/depparser/arcstandard/action_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

Decoder::Decoder(int nr, int beam_size, UpdateStrategy strategy, Weight* weight)
  : nr_deprels(nr),
  TransitionSystem<Action, State, ScoreContext, Weight>(beam_size, strategy, weight) {
}

void
Decoder::get_possible_actions(const State& source,
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

void
Decoder::transit(const State& source, const Action& act, const floatval_t& score,
    State* target) {
  int deprel;
  if (ActionUtils::is_shift(act)) {
    target->shift(source);
  } else if (ActionUtils::is_left_arc(act, deprel)) {
    target->left_arc(source, deprel);
  } else if (ActionUtils::is_right_arc(act, deprel)) {
    target->right_arc(source, deprel);
  } else {
    _ERROR << "unknown transition";
    exit(1);
  }
  target->score = score;
}

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
