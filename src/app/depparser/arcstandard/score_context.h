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
    const std::vector<form_t>& forms = state.ref->forms;
    const std::vector<postag_t>& postags = state.ref->postags;

    int S0 = state.top0;
    S0w = S0ldw = S0l2dw = S0rdw = S0r2dw = 0;
    S0p = S0ldp = S0l2dp = S0rdp = S0r2dp = 0;
    S0ldl = S0rdl = S0l2dl = S0r2dl = 0;
    if (S0 >= 0) {
      S0w = forms[S0]; S0p = postags[S0];
      S0la = state.nr_left_children[S0]; S0ra = state.nr_right_children[S0];

      int S0ld = state.left_most_child[S0];
      if (S0ld >= 0) {
        S0ldw = forms[S0ld]; S0ldp = postags[S0ld]; S0ldl = state.deprels[S0ld];
      }

      int S0rd = state.right_most_child[S0];
      if (S0rd >= 0) {
        S0rdw = forms[S0rd]; S0rdp = postags[S0rd]; S0rdl = state.deprels[S0rd];
      }

      int S0l2d = state.left_2nd_most_child[S0];
      if (S0l2d >= 0) {
        S0l2dw = forms[S0l2d]; S0l2dp = postags[S0l2d]; S0l2dl = state.deprels[S0l2d];
      }

      int S0r2d = state.right_2nd_most_child[S0];
      if (S0r2d >= 0) {
        S0r2dw = forms[S0r2d]; S0r2dp = postags[S0r2d]; S0r2dl = state.deprels[S0r2d];
      }
    }

    int S1 = state.top1;
    S1w = S1ldw = S1l2dw = S1rdw = S1r2dw = 0;
    S1p = S1ldp = S1l2dp = S1rdp = S1r2dp = 0;
    S1ldl = S1rdl = S1l2dl = S1r2dl = 0;
    if (S1 >= 0) {
      S1w = forms[S1]; S1p = postags[S1];
      S1la = state.nr_left_children[S1]; S1ra = state.nr_right_children[S1];

      int S1ld = state.left_most_child[S1];
      if (S1ld >= 0) {
        S1ldw = forms[S1ld]; S1ldp = postags[S1ld]; S1ldl = state.deprels[S1ld];
      }

      int S1rd = state.right_most_child[S1];
      if (S1rd >= 0) {
        S1rdw = forms[S1rd]; S1rdp = postags[S1rd]; S1rdl = state.deprels[S1rd];
      }

      int S1l2d = state.left_2nd_most_child[S1];
      if (S1l2d >= 0) {
        S1l2dw = forms[S1l2d]; S1l2dp = postags[S1l2d]; S1l2dl= state.deprels[S1l2d];
      }

      int S1r2d = state.right_2nd_most_child[S1];
      if (S1r2d >= 0) {
        S1r2dw = forms[S1r2d]; S1r2dp = postags[S1r2d]; S1r2dl= state.deprels[S1r2d];
      }
    }

    DistS0S1 = 0;
    if (S0 >= 0 && S1 >= 0) {
      DistS0S1 = 0;
    }

    N0w = N0p = 0;
    int N0 = (state.buffer < state.ref->size()) ? state.buffer: -1;
    if (N0 >= 0) {
      N0w = forms[N0]; N0p = postags[N0];
    }

    N1w = N1p = 0;
    int N1 = (state.buffer+ 1 < state.ref->size())? state.buffer+ 1: -1;
    if (N1 >= 0) {
      N1w = forms[N1]; N1p = postags[N1];
    }
  };

  form_t   S0w, S0ldw, S0rdw, S0l2dw, S0r2dw;
  form_t   S1w, S1ldw, S1rdw, S1l2dw, S1r2dw;
  form_t   N0w;
  form_t   N1w;

  postag_t S0p, S0ldp, S0rdp, S0l2dp, S0r2dp;
  postag_t S1p, S1ldp, S1rdp, S1l2dp, S1r2dp;
  postag_t N0p;
  postag_t N1p;

  deprel_t      S0ldl, S0rdl, S0l2dl, S0r2dl;
  deprel_t      S1ldl, S1rdl, S1l2dl, S1r2dl;

  int           S0la,  S0ra,  S1la,   S1ra;
  int           DistS0S1;
};

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_SCORE_CONTEXT_H__
