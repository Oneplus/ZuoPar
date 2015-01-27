#include <boost/assert.hpp>
#include "types/semchunks.h"
#include "experimental/acl2015/multi_predicate_srl1/action.h"
#include "experimental/acl2015/multi_predicate_srl1/action_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

bool ActionUtils::is_O(const Action& act) {
  return (act.action_name == kSemanticChunkOuterTag);
}

bool ActionUtils::is_B(const Action& act, tag_t& tag) {
  if (act.action_name >= kSemanticChunkBeginTag &&
      act.action_name < kSemanticChunkInterTag) {
    tag = act.action_name - kSemanticChunkBeginTag;
    return true;
  }
  return false;
}

bool ActionUtils::is_I(const Action& act, tag_t& tag) {
  if (act.action_name >= kSemanticChunkInterTag) {
    tag = act.action_name - kSemanticChunkInterTag;
    return true;
  }
  return false;
}

void ActionUtils::get_oracle_actions(const SemanticChunks& instance,
    std::vector<ActionCollection>& actions) {
  actions.clear();

  for (std::size_t i = 0; i < instance.size(); ++ i) {
    ActionCollection action_collection;
    for (std::size_t j = 0; j < instance.nr_predicates(); ++ j) {
      const std::vector<tag_t> tags = instance.predicates[j].second;
      tag_t tag = tags[i];
      if (tag == kSemanticChunkOuterTag) {
        action_collection.push_back(ActionFactory::make_O());
      } else if (tag > kSemanticChunkBeginTag && tag < kSemanticChunkInterTag) {
        action_collection.push_back(
            ActionFactory::make_B(tag - kSemanticChunkBeginTag));
      } else if (tag > kSemanticChunkInterTag) {
        action_collection.push_back(
            ActionFactory::make_I(tag - kSemanticChunkInterTag));
      } else {
        BOOST_ASSERT_MSG(false, "Illegal tag!");
      }
    }
    actions.push_back(action_collection);
  }
}

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar
