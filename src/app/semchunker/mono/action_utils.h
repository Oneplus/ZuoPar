#ifndef __ZUOPAR_APP_SEMCHUNKER_MONO_ACTION_UTILS_H__
#define __ZUOPAR_APP_SEMCHUNKER_MONO_ACTION_UTILS_H__

#include <vector>
#include "types/semchunks.h"
#include "app/semchunker/mono/action.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

class ActionUtils {
public:
  /**
   * Calculate the orcale action sequence in the arcstandard transition system.
   *
   *  @param[in]  instance      The input reference instance.
   *  @param[in]  predicate_id  The predicate index.
   *  @param[out] actions       The oracle transition sequence.
   */
  static void get_oracle_actions(const MonoSemanticChunks& instance,
      std::vector<Action>& actions);

  /**
   * Judge if the input action is a Other action.
   *
   *  @param[in]  act   The action.
   *  @return     bool  Return true on the action being a shift action, otherwise
   *                    false.
   */
  static bool is_O(const Action& act);

  //! Judge if the input action is a Begin action.
  static bool is_B(const Action& act, tag_t& tag);

  //! Judge if the input action is a Inter action.
  static bool is_I(const Action& act, tag_t& tag);
};

} //  namespace monopredicate
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEMCHUNKER_MONO_ACTION_UTILS_H__
