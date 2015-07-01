#include <boost/assert.hpp>
#include "types/semchunks.h"
#include "experimental/multi_predicate_srl/action.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

std::ostream& operator<<(std::ostream& os, const Action& act) {
  if (act.name() == 0) {
    os << "None";
  } else if (act.name() == kSemanticChunkOuterTag) {
    os << "O";
  } else if (act.name() >= kSemanticChunkBeginTag &&
      act.name() < kSemanticChunkInterTag) {
    os << "B~" << act.name() - kSemanticChunkBeginTag;
  } else {
    os << "I~" << act.name() - kSemanticChunkInterTag;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const ActionCollection& act) {
  os << "(";
  for (size_t i = 0; i < act.size(); ++ i) {
    os << act[i]; if (i + 1 < act.size()) { os << ", "; }
  }
  os << ")";
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

} //  namespace multipredicatesrl
} //  namespace experimental
} //  namespace zuopar
