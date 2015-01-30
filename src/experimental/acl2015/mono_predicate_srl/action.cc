#include <boost/assert.hpp>
#include "types/semchunks.h"
#include "experimental/acl2015/mono_predicate_srl/action.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

std::ostream& operator<<(std::ostream& os, const Action& act) {
  if (act.action_name == 0) {
    os << "NONE";
  } else if (act.action_name == kSemanticChunkOuterTag) {
    os << "O";
  } else if (act.action_name > kSemanticChunkBeginTag &&
      act.action_name < kSemanticChunkInterTag) {
    os << "B~" << act.action_name - kSemanticChunkBeginTag;
  } else {
    os << "I~" << act.action_name - kSemanticChunkInterTag;
  }
  return os;
}

Action ActionFactory::make_O() {
  return Action(kSemanticChunkOuterTag);
}

Action ActionFactory::make_B(const tag_t& tag) {
  return Action(kSemanticChunkBeginTag+ tag);
}

Action ActionFactory::make_I(const tag_t& tag) {
  return Action(kSemanticChunkInterTag+ tag);
}

} //  namespace monopredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar
