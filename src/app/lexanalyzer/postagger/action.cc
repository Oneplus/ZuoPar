#include <boost/assert.hpp>
#include "app/lexanalyzer/postagger/action.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

Action
ActionFactory::make_shift(const postag_t& postag) {
  return Action(Action::kShift, postag);
}

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
