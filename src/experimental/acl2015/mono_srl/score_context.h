#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_SCORE_CONTEXT_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_SCORE_CONTEXT_H__

#include <vector>
#include "types/common.h"
#include "types/semchunks.h"
#include "engine/token_alphabet.h"
#include "experimental/acl2015/mono_srl/state.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoSRL {

namespace eg = ZuoPar::Engine;

class ScoreContext {
public:
  /**
   * The ScoreContext constructor
   *
   *  @param[in]  state   The State
   */
  ScoreContext(const State& state)
    : instance(state.ref) {
  };

  form_t    w_2, w_1, w0, w1, w2;
  postag_t  p_2, p_1, p0, p1, p2;
  form_t    w_pred;
  postag_t  p_pred, p_pred_1, p_pred1;
  int postition;
  int distance;

  const MonoSemanticChunks* instance;
};

} //  namespace monosrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_SCORE_CONTEXT_H__
