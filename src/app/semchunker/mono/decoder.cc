#include "app/semchunker/mono/decoder.h"
#include "app/semchunker/mono/action.h"
#include "app/semchunker/mono/action_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

Decoder::Decoder(int nr, int beam_size, bool avg, UpdateStrategy strategy, Weight* weight)
  : nr_tags(nr),
  TransitionSystem<Action, State, ScoreContext, Weight>(beam_size, avg, strategy, weight) {
}

void
Decoder::get_possible_actions(const State& source,
    std::vector<Action>& actions) {
  actions.clear();
  int buffer = source.buffer;
  if (buffer == 0) {
    actions.push_back(ActionFactory::make_O());
    for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
      actions.push_back(ActionFactory::make_B(p));
    }
  } else {
    tag_t tag;
    if (ActionUtils::is_B(source.last_action, tag)) {
      actions.push_back(ActionFactory::make_O());
      actions.push_back(ActionFactory::make_I(tag));
      for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
        actions.push_back(ActionFactory::make_B(p));
      }
    } else if (ActionUtils::is_I(source.last_action, tag)) {
      actions.push_back(ActionFactory::make_O());
      actions.push_back(ActionFactory::make_I(tag));
      for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
        actions.push_back(ActionFactory::make_B(p));
      }
    } else if (ActionUtils::is_O(source.last_action)) {
      actions.push_back(ActionFactory::make_O());
      for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
        actions.push_back(ActionFactory::make_B(p));
      }
    } else {
      BOOST_ASSERT_MSG(false, "unknown transition");
    }
  }
}

void
Decoder::transit(const State& source, const Action& act, const floatval_t& score,
    State* target) {
  tag_t tag;
  if (ActionUtils::is_O(act)) { target->O(source); }
  else if (ActionUtils::is_B(act, tag)) { target->B(source, tag); }
  else if (ActionUtils::is_I(act, tag)) { target->I(source, tag); }
  else { BOOST_ASSERT_MSG(false, "unknown transition"); }
  target->score = score;
}

bool Decoder::terminated() { return false; }

} //  namespace monopredicatesrl
} //  namespace experimental
} //  namespace zuopar
