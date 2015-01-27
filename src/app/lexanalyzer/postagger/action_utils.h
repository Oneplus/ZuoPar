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

  /**
   * Judge if the input action is a shift action.
   *
   *  @param[in]  act     The action.
   *  @param[out] postag  The postag embed in this action.
   *  @return     bool    Return true on the action being a shift action,
   *                      otherwise false.
   */
  static bool is_shift(const Action& act, postag_t& postag);
};

} //  end for postagger
} //  end for lexicalanalyzer
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_ACTION_UTILS_H__
