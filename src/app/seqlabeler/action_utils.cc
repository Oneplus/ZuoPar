#include <boost/assert.hpp>
#include "app/seqlabeler/action.h"
#include "app/seqlabeler/action_utils.h"

namespace ZuoPar {
namespace SequenceLabeler {

bool
ActionUtils::is_shift(const Action& act, postag_t& postag) {
  if (act.action_name == Action::kShift) {
    postag = act.deprel;
    return true;
  }
  return false;
}

void
ActionUtils::get_oracle_actions(const SequenceInstance& instance,
    std::vector<Action>& actions) {
  actions.clear();
  for (int i = 0; i < instance.size(); ++ i) {
    tag_t tag = instance[i].tag;
    actions.push_back(ActionFactory::make_shift(tag));
  }
}

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
