#include <boost/assert.hpp>
#include "app/semchunker/mono/action.h"
#include "app/semchunker/mono/action_utils.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

bool ActionUtils::is_O(const Action& act) {
  return (act.name() == Action::kO);
}

bool ActionUtils::is_B(const Action& act, tag_t& tag) {
  if (act.name() == Action::kB) { tag = act.rel(); return true; }
  return false;
}

bool ActionUtils::is_I(const Action& act, tag_t& tag) {
  if (act.name() == Action::kI) { tag = act.rel(); return true; }
  return false;
}

void ActionUtils::get_oracle_actions(const MonoSemanticChunks& instance,
    std::vector<Action>& actions) {
  actions.clear();
  for (const tag_t& tag: instance.predicate.second) {
    if (tag == kSemanticChunkOuterTag) {
      actions.push_back(ActionFactory::make_O());
    } else if (tag > kSemanticChunkBeginTag && tag < kSemanticChunkInterTag) {
      actions.push_back(ActionFactory::make_B(tag - kSemanticChunkBeginTag));
    } else if (tag > kSemanticChunkInterTag) {
      actions.push_back(ActionFactory::make_I(tag - kSemanticChunkInterTag));
    } else {
      BOOST_ASSERT_MSG(false, "Illegal tag!");
    }
  }
}

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar
