#include <boost/assert.hpp>
#include "experimental/acl2015/mono_predicate_srl/action.h"
#include "experimental/acl2015/mono_predicate_srl/action_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

bool ActionUtils::is_O(const Action& act) {
  return (act.action_name == kSemanticChunkOuterTag);
}

bool ActionUtils::is_B(const Action& act, tag_t& tag) {
  if (act.action_name > kSemanticChunkBeginTag &&
      act.action_name < kSemanticChunkInterTag) {
    tag = act.action_name - kSemanticChunkBeginTag;
    return true;
  }
  return false;
}

bool ActionUtils::is_I(const Action& act, tag_t& tag) {
  if (act.action_name > kSemanticChunkInterTag) {
    tag = act.action_name - kSemanticChunkInterTag;
    return true;
  }
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

} //  namespace monopredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar
