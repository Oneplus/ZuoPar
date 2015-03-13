#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_ACTION_UTILS_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_ACTION_UTILS_H__

#include <vector>
#include "types/postag.h"
#include "app/lexanalyzer/postagger/action.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

class ActionUtils {
public:
  /**
   * Calculate the orcale action sequence in the arcstandard transition system.
   *
   *  @param[in]  instance  The input reference instance.
   *  @param[out] actions   The oracle transition sequence.
   */
  static void get_oracle_actions(const Postag& instance,
      std::vector<Action>& actions);
};

} //  end for postagger
} //  end for lexicalanalyzer
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_ACTION_UTILS_H__
