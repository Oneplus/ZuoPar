#include <boost/assert.hpp>
#include "app/lexanalyzer/wordseg/action.h"
#include "app/lexanalyzer/wordseg/action_utils.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

bool ActionUtils::is_split(const Action& act)  { return act.name() == Action::kSplit;  }
bool ActionUtils::is_append(const Action& act) { return act.name() == Action::kAppend; }

void ActionUtils::get_oracle_actions(const Segmentation& instance,
    std::vector<Action>& actions) {
  std::string word = instance.words[0];
  std::string ch = "";
  for (int i = 0, j = 0; i < instance.nr_chars(); ++ i) {
    ch += instance.chars[i];
    if (word == ch) {
      actions.push_back(ActionFactory::make_split());
      ++ j;
      if (j < instance.words.size()) { word = instance.words[j]; }
      ch = "";
    } else {
      actions.push_back(ActionFactory::make_append());
    }
  }
}

} //  end for namespace chinesewordsegmentor
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
