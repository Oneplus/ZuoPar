#include <boost/assert.hpp>
#include "app/seqlabeler/action.h"
#include "app/seqlabeler/action_utils.h"

namespace ZuoPar {
namespace SequenceLabeler {

void
ActionUtils::get_oracle_actions(const SequenceInstance& instance,
    std::vector<Action>& actions) {
  actions.clear();
  for (const SequenceItem& item: instance) {
    actions.push_back(ActionFactory::make(item.tag));
  }
}

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
