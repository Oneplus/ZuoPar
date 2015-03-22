#ifndef __ZUOPAR_APP_LEXANALYZER_WORDSEG_ACTION_UTILS_H__
#define __ZUOPAR_APP_LEXANALYZER_WORDSEG_ACTION_UTILS_H__

#include <vector>
#include "types/segmentation.h"
#include "app/lexanalyzer/wordseg/action.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

class ActionUtils {
public:
  static bool is_split(const Action& act);  //! Shorthand for act.name() == kSplit
  static bool is_append(const Action& act); //! Shorthand for act.name() == kAppend

  /**
   * Calculate the orcale action sequence in the arcstandard transition system.
   *
   *  @param[in]  instance  The input reference instance.
   *  @param[out] actions   The oracle transition sequence.
   */
  static void get_oracle_actions(const Segmentation& instance,
      std::vector<Action>& actions);
};

} //  end for chinesewordsegmentor
} //  end for lexicalanalyzer
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_WORDSEG_ACTION_UTILS_H__
