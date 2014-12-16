#include <boost/assert.hpp>
#include "app/seqlabeler/action.h"

namespace ZuoPar {
namespace SequenceLabeler {

Action
ActionFactory::make_shift(const tag_t& tag) {
  return Action(Action::kShift, tag);
}

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
