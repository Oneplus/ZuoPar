#ifndef __ZUOPAR_APP_DEPPARSER_CHOI2013_DECODER_H__
#define __ZUOPAR_APP_DEPPARSER_CHOI2013_DECODER_H__

#include "engine/token_alphabet.h"
#include "system/options.h"
#include "system/system.h"
#include "app/depparser/choi2013/state.h"
#include "app/depparser/choi2013/action.h"
#include "app/depparser/choi2013/score_context.h"
#include "app/depparser/choi2013/action.h"
#include "app/depparser/choi2013/weight.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

namespace eg = ZuoPar::Engine;

class Decoder: public TransitionSystem<Action, State, ScoreContext, Weight> {
public:
  enum RootPosition {
    kNone,
    kLeft,
    kRight
  };

  /**
   * The decoder constructor.
   *
   *  @param[in]  nr              The number of dependency relations.
   *  @param[in]  root            The tag for root.
   *  @param[in]  beam_size       The size of beam.
   *  @param[in]  update_strategy The update strategy.
   *  @param[in]  weight          The pointer to the weight.
   */
  Decoder(int nr, int root, int position,
      int beam_size, bool avg, UpdateStrategy update_strategy, Weight* weight);

  //! Implement arc standard get possible actions.
  void get_possible_actions(const State& source,
      std::vector<Action>& actions);

  //! Implement arc standard transition.
  void transit(const State& source, const Action& act, const floatval_t& score,
      State* target);

  bool terminated();
private:
  int nr_deprels;     //! The number of dependency relations, NONE, BEGIN, END included.
  int root_tag;       //! The tag of root.
  int root_position;  //!
};

} //  end for namespace choi2013
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCEAGER_DECODER_H__