#ifndef __ZUOPAR_APP_DEPPARSER_CHOI2013_SCORE_CONTEXT_H__
#define __ZUOPAR_APP_DEPPARSER_CHOI2013_SCORE_CONTEXT_H__

#include <vector>
#include "types/common.h"
#include "app/depparser/choi2013/state.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

class ScoreContext {
public:
  ScoreContext(const State& state);

  form_t   S0w, S0ldw, S0rdw, S0l2dw, S0r2dw, S1w;
  form_t   N0w, N0ldw, N0l2dw;
  form_t   S0hw, S0h2w, N0hw, N0h2w;
  form_t   S0L2w, S0L1w, S0R1w, S0R2w, N0L2w, N0L1w, N0R1w, N0R2w;

  postag_t S0p, S0ldp, S0rdp, S0l2dp, S0r2dp, S1p;
  postag_t N0p, N0ldp, N0l2dp;
  postag_t S0hp, S0h2p, N0hp, N0h2p;
  postag_t S0L2p, S0L1p, S0R1p, S0R2p, N0L2p, N0L1p, N0R1p, N0R2p;

  deprel_t S0ldl, S0rdl, S0l2dl, S0r2dl;
  deprel_t N0ldl,        N0l2dl;
  deprel_t S0hl, S0h2l, N0hl, N0h2l;

  int S0la,  S0ra,  N0la;
  int S0lsetl,S0lseth, S0rsetl, S0rseth ,N0lsetl, N0lseth;
  int DistS0N0;

  int S0IsBegin, N0IsEnd, S0N0Adjacent;
  const std::vector<int>* S0Feat;
  const std::vector<int>* N0Feat;
};

} //  end for namespace choi2013
} //  end for namespace dependencyparser
} //  end for namespace zuopar
#endif  //  end for __ZUOPAR_APP_DEPPARSER_CHOI2013_SCORE_CONTEXT_H__
