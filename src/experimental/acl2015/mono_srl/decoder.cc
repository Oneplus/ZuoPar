#include "experimental/acl2015/mono_srl/decoder.h"
#include "experimental/acl2015/mono_srl/action.h"
#include "experimental/acl2015/mono_srl/action_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoSRL {

Decoder::Decoder(int nr, int beam_size, bool early_update, Weight* weight)
  : nr_tags(nr),
  TransitionSystem<Action, State, ScoreContext, Weight>(beam_size, early_update, weight) {
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
    actions.push_back(ActionFactory::make_O());
    for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
      actions.push_back(ActionFactory::make_B(p));
    }

    tag_t tag;
    if (ActionUtils::is_B(source.last_action, tag)) {
      actions.push_back(ActionFactory::make_I(tag));
    } else if (ActionUtils::is_I(source.last_action, tag)) {
      actions.push_back(ActionFactory::make_I(tag));
    } else {
      for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
        actions.push_back(ActionFactory::make_I(p));
      }
    }
  }
}

void
Decoder::transit(const State& source, const Action& act, const floatval_t& score,
    State* target) {
  tag_t tag;
  if (ActionUtils::is_O(act)) {
    target->O(source);
  } else if (ActionUtils::is_B(act, tag)) {
    target->B(source, tag);
  } else if (ActionUtils::is_I(act, tag)) {
    target->I(source, tag);
  } else {
    _ERROR << "unknown transition";
    exit(1);
  }
  target->score = score;
}

} //  namespace monosrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar
