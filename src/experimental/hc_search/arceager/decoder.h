#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_DECODER_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_DECODER_H__

#include "engine/token_alphabet.h"
#include "system/options.h"
#include "system/system.h"
#include "experimental/hc_search/arceager/state.h"
#include "experimental/hc_search/arceager/action.h"
#include "experimental/hc_search/arceager/score_context.h"
#include "experimental/hc_search/arceager/action.h"
#include "experimental/hc_search/arceager/weight.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

namespace eg = ZuoPar::Engine;

class Decoder
  : public TransitionSystem<Action, State, HeuristicScoreContext, HeuristicWeight> {
public:
  /**
   * The decoder constructor.
   *
   *  @param[in]  nr              The number of dependency relations.
   *  @param[in]  beam_size       The size of beam.
   *  @param[in]  update_strategy The update strategy.
   *  @param[in]  weight          The pointer to the weight.
   */
  Decoder(int nr, int beam_size, bool avg, UpdateStrategy update_strategy, HeuristicWeight* weight);

  //! Implement arc standard get possible actions.
  void get_possible_actions(const State& source,
      std::vector<Action>& actions);

  //! Implement arc standard transition.
  void transit(const State& source, const Action& act, const floatval_t& score,
      State* target);

  //! Get the results in beam.
  void get_results_in_beam(std::vector<const State*>& results, int round);

private:
  int nr_deprels;
};

} //  end for namespace arceager
} //  end for namespace dependencyparser
} //  end for namespace zuopar


#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_DECODER_H__
