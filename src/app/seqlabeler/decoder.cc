#include "app/seqlabeler/decoder.h"
#include "app/seqlabeler/action_utils.h"

namespace ZuoPar {
namespace SequenceLabeler {

Decoder::Decoder(int nr, int beam_size, Weight* weight)
  : nr_tags(nr),
  TransitionSystem<Action, State, ScoreContext, Weight>(beam_size, weight) {
}

void
Decoder::get_possible_actions(const State& source,
    std::vector<Action>& actions) {
  actions.clear();
  for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
    actions.push_back(ActionFactory::make_shift(p));
  }
}

void
Decoder::transit(const State& source, const Action& act, const floatval_t& score,
    State* target) {
  tag_t tag;
  if (ActionUtils::is_shift(act, tag)) {
    target->shift(source, tag);
  } else {
    _ERROR << "unknown transition";
    exit(1);
  }
  target->score = score;
}

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
