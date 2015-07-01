#ifndef __ZUOPAR_APP_SEMCHUNKER_MONO_SCORE_CONTEXT_H__
#define __ZUOPAR_APP_SEMCHUNKER_MONO_SCORE_CONTEXT_H__

#include <vector>
#include <boost/assert.hpp>
#include "types/common.h"
#include "types/semchunks.h"
#include "engine/token_alphabet.h"
#include "app/semchunker/mono/state.h"
#include "app/semchunker/mono/action_utils.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

namespace eg = ZuoPar::Engine;

class ScoreContext {
public:
  /**
   * The ScoreContext constructor
   *
   *  @param[in]  state   The State
   */
  ScoreContext(const State& state);

  form_t    w_2, w_1, w0, w1, w2;
  postag_t  p_2, p_1, p0, p1, p2;
  form_t    w_pred;
  postag_t  p_pred, p_pred_1, p_pred1;
  tag_t     vc_pred;
  tag_t     t_1;
  int position;
  int dist;
  int label_set;
  const std::string& path;
};

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEMCHUNKER_MONO_SCORE_CONTEXT_H__
