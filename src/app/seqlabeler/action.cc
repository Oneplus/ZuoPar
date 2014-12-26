#include <boost/assert.hpp>
#include "app/seqlabeler/action.h"

namespace ZuoPar {
namespace SequenceLabeler {

Action
ActionFactory::make(const tag_t& tag) {
  return Action(tag);
}

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
