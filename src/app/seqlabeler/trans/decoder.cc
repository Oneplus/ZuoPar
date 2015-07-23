#include "app/seqlabeler/trans/decoder.h"
#include "app/seqlabeler/trans/action_utils.h"

namespace ZuoPar {
namespace SequenceLabeler {

Decoder::Decoder(int nr, const std::vector< std::vector<bool> >& trans_,
    int beam_size, bool avg, UpdateStrategy strategy, Weight* weight)
  : nr_tags(nr),
  trans(trans_),
  TransitionSystem<Action, State, Weight>(beam_size, avg, strategy, weight) {
}

void Decoder::get_possible_actions(const State& source,
    std::vector<Action>& actions) {
  actions.clear();
  tag_t s = 1;
  if (source.previous != NULL) { s = source.last_action.name(); }
  for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
    if (trans[s][p]) {
      _TRACE << "decoder: legal transition: " << s << " -> " << p;
      actions.push_back(ActionFactory::make(p));
    }
  }
}

void Decoder::transit(const State& source, const Action& act, const floatval_t& score,
    State* target) {
  tag_t tag = act.name();
  target->tag(source, tag);
  target->score = score;
}

bool Decoder::terminated() {
  return false;
}

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
