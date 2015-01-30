#include <boost/assert.hpp>
#include "app/seqlabeler/action.h"

namespace ZuoPar {
namespace SequenceLabeler {

std::ostream& operator<<(std::ostream& os, const Action& act) {
  os << "TAG~" << act.action_name;
  return os;
}

Action ActionFactory::make(const tag_t& tag) {
  return Action(tag);
}

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
