#include <boost/assert.hpp>
#include "types/semchunks.h"
#include "experimental/multi_predicate_srl/action.h"
#include "experimental/multi_predicate_srl/action_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

bool ActionUtils::is_O(const Action& act) {
  return (act.name() == kSemanticChunkOuterTag);
}

bool ActionUtils::is_B(const Action& act, tag_t& tag) {
  if (act.name() >= kSemanticChunkBeginTag &&
      act.name() < kSemanticChunkInterTag) {
    tag = act.name() - kSemanticChunkBeginTag;
    return true;
  }
  return false;
}

bool ActionUtils::is_I(const Action& act, tag_t& tag) {
  if (act.name() >= kSemanticChunkInterTag) {
    tag = act.name() - kSemanticChunkInterTag;
    return true;
  }
  return false;
}

int ActionUtils::max(int nr_tags, bool erase) {
  return (nr_tags- (erase? 3: 0))* 2 + 1;
}

int ActionUtils::compress(const Action& act, int nr_tags, bool erase) {
  if (act.name() == kSemanticChunkOuterTag) {
    return 0;
  } else if (act.name() > kSemanticChunkInterTag) {
    return (
        ((act.name() - kSemanticChunkInterTag)- (erase? 3: 0)) // compressed tag id
        + (nr_tags- (erase? 3:0)+ 1)                               // compressed nr tags
        );
  } else {
    return ((act.name() - kSemanticChunkBeginTag) - (erase? 3: 0) + 1);
  }
  return -1;
}

Action ActionUtils::decompress(int act, int nr_tags, bool erase) {
  int compressed_nr_tags = nr_tags - (erase? 3: 0);
  if (act == 0) {
    return ActionFactory::make_O();
  } else if (act > compressed_nr_tags) {
    tag_t tag = act - compressed_nr_tags - 1 + (erase? 3: 0);
    return ActionFactory::make_I(tag);
  } else {
    tag_t tag = act - 1 + (erase? 3: 0);
    return ActionFactory::make_B(tag);
  }
}

tag_t ActionUtils::tag(const Action& act) {
  if (act.name() == kSemanticChunkOuterTag) {
    return 0;
  } else if (act.name() > kSemanticChunkInterTag) {
    return act.name() - kSemanticChunkInterTag;
  } else {
    return act.name() - kSemanticChunkBeginTag;
  }
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
} //  namespace experimental
} //  namespace zuopar
