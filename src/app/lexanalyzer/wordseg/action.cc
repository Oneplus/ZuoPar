#include <boost/assert.hpp>
#include "app/lexanalyzer/wordseg/action.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

Action ActionFactory::make_split()  { return Action(Action::kSplit);  }
Action ActionFactory::make_append() { return Action(Action::kAppend); }

std::ostream& operator<<(std::ostream& os, const Action& act) {
  os << (act.name() == Action::kSplit ? "S": "A");
  return os;
}

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
