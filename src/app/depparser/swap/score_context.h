#ifndef __ZUOPAR_APP_DEPPARSER_SWAP_SCORE_CONTEXT_H__
#define __ZUOPAR_APP_DEPPARSER_SWAP_SCORE_CONTEXT_H__

#include <vector>
#include "types/common.h"
#include "utils/math/fast_binned.h"
#include "app/depparser/swap/state.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

class ScoreContext {
public:
  ScoreContext(const State& state);

  form_t   S0w, S0ldw, S0rdw, S0l2dw, S0r2dw, S0L2w, S0L1w, S0R1w, S0R2w;
  form_t   S1w, S1ldw, S1rdw, S1l2dw, S1r2dw, S1L2w, S1L1w, S1R1w, S1R2w;
  form_t   N0w, N0L2w, N0L1w, N0R1w, N0R2w;

  postag_t S0p, S0ldp, S0rdp, S0l2dp, S0r2dp, S0L2p, S0L1p, S0R1p, S0R2p;
  postag_t S1p, S1ldp, S1rdp, S1l2dp, S1r2dp, S1L2p, S1L1p, S1R1p, S1R2p;
  postag_t N0p, N0L2p, N0L1p, N0R1p, N0R2p;

  deprel_t      S0ldl, S0rdl, S0l2dl, S0r2dl;
  deprel_t      S1ldl, S1rdl, S1l2dl, S1r2dl;

  int S0la,  S0ra,  S1la,   S1ra;
  int S0lsetl, S0lseth, S0rsetl, S0rseth;
  int S1lsetl, S1lseth, S1rsetl, S1rseth;
  int DistS0S1;

  int S1IsBegin, S0IsEnd, N0IsEnd, S1S0Adjancet;
  const std::vector<int>* S1Feat;
  const std::vector<int>* S0Feat;
  const std::vector<int>* N0Feat;
};

} //  end for namespace swap
} //  end for namespace dependencyparser
} //  end for namespace zuopar
#endif  //  end for __ZUOPAR_APP_DEPPARSER_SWAP_SCORE_CONTEXT_H__
