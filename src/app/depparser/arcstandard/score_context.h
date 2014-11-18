#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_SCORE_CONTEXT_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_SCORE_CONTEXT_H__

#include <vector>
#include "types/common.h"
#include "app/depparser/arcstandard/state.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

class ScoreContext {
public:
  /**
   * The ScoreContext constructor
   *
   *  @param[in]  state   The State
   */
  ScoreContext(const State& state) {
    int S0 = state.top0;
    const std::vector<form_t>& forms = state.ref->forms;
    const std::vector<postag_t>& postags = state.ref->postags;

    if (S0 >= 0) {
      S0w = forms[S0];
      S0p = postags[S0];
    }
  };

  form_t S0w;
  postag_t S0p;
};

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_SCORE_CONTEXT_H__
