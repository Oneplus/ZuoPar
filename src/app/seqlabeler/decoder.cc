#include "app/seqlabeler/decoder.h"
#include "app/seqlabeler/action_utils.h"

namespace ZuoPar {
namespace SequenceLabeler {

Decoder::Decoder(int nr, const std::vector< std::vector<bool> >& trans_,
    int beam_size, UpdateStrategy strategy, Weight* weight)
  : nr_tags(nr),
  trans(trans_),
  TransitionSystem<Action, State, ScoreContext, Weight>(beam_size, strategy, weight) {
}

void
Decoder::get_possible_actions(const State& source,
    std::vector<Action>& actions) {
  actions.clear();
  tag_t s = 1;
  if (source.previous != NULL) { s = ActionUtils::tag(source.last_action); }
  for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
    if (trans[s][p]) {
      _TRACE << "decoder: legal transition: " << s << " -> " << p;
      actions.push_back(ActionFactory::make(p));
    }
  }
}

void
Decoder::transit(const State& source, const Action& act, const floatval_t& score,
    State* target) {
  tag_t tag = ActionUtils::tag(act);
  target->tag(source, tag);
  target->score = score;
}

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
