#ifndef __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_ACTION_UTILS_H__
#define __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_ACTION_UTILS_H__

#include <vector>
#include "types/semchunks.h"
#include "experimental/multi_predicate_srl/action.h"

namespace ZuoPar {
namespace Experimental {
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

  //!
  static int max(int nr_tags, bool erase_default = true);

  /**
   * Compress the action into the integer.
   *
   *  @param[in]  act     The action.
   *  @param[in]  nr_tags The total number of tag categories.
   *  @param[in]  erase   If erase is true, this function will erase the default
   *                      tags: NONE(0), BEGIN(1), END(2).
   */
  static int compress(const Action& act, int nr_tags, bool erase = true);

  /**
   * Decompress the integer into the Action.
   *
   *  @param[in]  act     The compressed integer action.
   *  @param[in]  nr_tags The total number of tag categories.
   *  @param[in]  erase   If erase is true, this function will erase the default
   *                      tags: NONE(0), BEGIN(1), END(2).
   */
  static Action decompress(int act, int nr_tags, bool erase = true);

  //
  static tag_t tag(const Action& act);

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
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_ACTION_UTILS_H__
