#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_ACTION_UTILS_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_ACTION_UTILS_H__

#include <vector>
#include "types/semchunks.h"
#include "experimental/acl2015/multi_predicate_srl1/action.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

class ActionUtils {
public:
  /**
   * Calculate the orcale action sequence in the .
   *
   *  @param[in]  instance      The input reference instance.
   *  @param[out] actions       The oracle transition sequence.
   */
  static void get_oracle_actions(const SemanticChunks& instance,
      std::vector<ActionCollection>& actions);

  /**
   * Judge if the input action is a shift action.
   *
   *  @param[in]  act   The action.
   *  @return     bool  Return true on the action being a shift action, otherwise
   *                    false.
   */
  static bool is_O(const Action& act);

  //! Judge if the input action is a B action.
  static bool is_B(const Action& act, tag_t& tag);

  //! Judge if the input action is a I action.
  static bool is_I(const Action& act, tag_t& tag);
};

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_ACTION_UTILS_H__
