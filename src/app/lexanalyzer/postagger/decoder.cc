#include "app/lexanalyzer/postagger/decoder.h"
#include "app/lexanalyzer/postagger/action_utils.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

Decoder::Decoder(int nr, int beam_size, Weight* weight)
  : nr_postags(nr),
  TransitionSystem<Action, State, ScoreContext, Weight>(
      beam_size, UpdateStrategy::kEarlyUpdate, weight) {
}

void
Decoder::get_possible_actions(const State& source,
    std::vector<Action>& actions) {
  actions.clear();
  for (postag_t p = eg::TokenAlphabet::END+ 1; p < nr_postags; ++ p) {
    actions.push_back(ActionFactory::make_shift(p));
  }
}

void
Decoder::transit(const State& source, const Action& act, const floatval_t& score,
    State* target) {
  int postag;
  if (ActionUtils::is_shift(act, postag)) {
    target->shift(source, postag);
  } else {
    _ERROR << "unknown transition";
    exit(1);
  }
  target->score = score;
}

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
