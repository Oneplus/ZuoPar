#include <boost/assert.hpp>
#include "types/semchunks.h"
#include "app/semchunker/mono/action.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

std::ostream& operator<<(std::ostream& os, const Action& act) {
  if (act.name() == Action::kNone)    { os << "NONE"; }
  else if (act.name() == Action::kO)  { os << "O"; }
  else if (act.name() == Action::kB)  { os << "B#" << act.rel(); }
  else if (act.name() == Action::kI)  { os << "I#" << act.rel(); }
  else                                { os << "UNK:" << act.name() << " " << act.rel(); }
  return os;
}

Action ActionFactory::make_O() { return Action(Action::kO, 0);}
Action ActionFactory::make_B(const tag_t& tag) { return Action(Action::kB, tag); }
Action ActionFactory::make_I(const tag_t& tag) { return Action(Action::kI, tag); }

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar
