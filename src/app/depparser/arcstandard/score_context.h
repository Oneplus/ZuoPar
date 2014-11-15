#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_SCORE_CONTEXT_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_SCORE_CONTEXT_H__

#include "types/common.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

class ScoreContext {
public:
  ScoreContext(const State& state) {};

  form_t S0w;
  postag_t S0p;
};

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_SCORE_CONTEXT_H__
