#include <boost/assert.hpp>
#include "app/seqlabeler/action.h"
#include "app/seqlabeler/action_utils.h"

namespace ZuoPar {
namespace SequenceLabeler {

tag_t
ActionUtils::tag(const Action& act) {
  return act.action_name;
}

void
ActionUtils::get_oracle_actions(const SequenceInstance& instance,
    std::vector<Action>& actions) {
  actions.clear();
  for (int i = 0; i < instance.size(); ++ i) {
    tag_t tag = instance[i].tag;
    actions.push_back(ActionFactory::make(tag));
  }
}

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
