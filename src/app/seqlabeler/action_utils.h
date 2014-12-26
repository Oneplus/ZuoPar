#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_ACTION_UTILS_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_ACTION_UTILS_H__

#include <vector>
#include "types/sequence_instance.h"
#include "app/seqlabeler/action.h"

namespace ZuoPar {
namespace SequenceLabeler {

class ActionUtils {
public:
  /**
   * Calculate the orcale action sequence in the arcstandard transition system.
   *
   *  @param[in]  instance  The input reference instance.
   *  @param[out] actions   The oracle transition sequence.
   */
  static void get_oracle_actions(const SequenceInstance& instance,
      std::vector<Action>& actions);

  static tag_t tag(const Action& act);
};

} //  end for lexicalanalyzer
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_ACTION_UTILS_H__
