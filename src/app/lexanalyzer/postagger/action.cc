#include <boost/assert.hpp>
#include "app/lexanalyzer/postagger/action.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

Action ActionFactory::make(const postag_t& postag) {
  return Action(postag);
}

std::ostream& operator<<(std::ostream& os, const Action& act) {
  os << act.name();
  return os;
}

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
