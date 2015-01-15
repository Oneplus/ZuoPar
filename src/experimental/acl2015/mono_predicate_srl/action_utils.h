#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_ACTION_UTILS_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_ACTION_UTILS_H__

#include <vector>
#include "types/semchunks.h"
#include "experimental/acl2015/mono_predicate_srl/action.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

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
   * Judge if the input action is a shift action.
   *
   *  @param[in]  act   The action.
   *  @return     bool  Return true on the action being a shift action, otherwise
   *                    false.
   */
  static bool is_O(const Action& act);

  //!
  static bool is_B(const Action& act, tag_t& tag);

  //!
  static bool is_I(const Action& act, tag_t& tag);
};

} //  namespace monopredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_ACTION_UTILS_H__
