#include <boost/assert.hpp>
#include "app/lexanalyzer/postagger/action.h"
#include "app/lexanalyzer/postagger/action_utils.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

void ActionUtils::get_oracle_actions(const Postag& instance,
    std::vector<Action>& actions) {
  actions.clear();
  for (int i = 0; i < instance.size(); ++ i) {
    postag_t postag = instance.postags[i];
    actions.push_back(ActionFactory::make(postag));
  }
}

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
