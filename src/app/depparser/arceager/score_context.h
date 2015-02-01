#ifndef __ZUOPAR_APP_DEPPARSER_ARCEAGER_SCORE_CONTEXT_H__
#define __ZUOPAR_APP_DEPPARSER_ARCEAGER_SCORE_CONTEXT_H__

#include <vector>
#include "types/common.h"
#include "utils/math/fast_binned.h"
#include "app/depparser/arceager/state.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

class ScoreContext {
public:
  /**
   * The ScoreContext constructor
   *
   *  @param[in]  state   The State
   */
  ScoreContext(const State& state):
    S0w(0), S0ldw(0), S0rdw(0), S0l2dw(0), S0r2dw(0),
    N0w(0), N0ldw(0), N0l2dw(0),
    N1w(0),
    N2w(0),
    S0hw(0),
    S0h2w(0),
    S0p(0), S0ldp(0), S0rdp(0), S0l2dp(0), S0r2dp(0),
    N0p(0), N0ldp(0), N0l2dp(0),
    N1p(0),
    N2p(0),
    S0hp(0),
    S0h2p(0),
    S0ldl(0), S0rdl(0), S0l2dl(0), S0r2dl(0),
    N0ldl(0), N0l2dl(0),
    S0hl(0),
    S0h2l(0),
    S0la(0), S0ra(0), N0la(0),
    S0lset(0), S0rset(0), N0lset(0),
    DistS0N0(0) {
    const std::vector<form_t>& forms = state.ref->forms;
    const std::vector<postag_t>& postags = state.ref->postags;
    int len = state.ref->size();

    int S0 = state.top0;
    if (S0 >= 0) {
      S0w = forms[S0]; S0p = postags[S0];
      S0la = state.nr_left_children[S0]; S0ra = state.nr_right_children[S0];
      S0lset = state.left_label_set[S0]; S0rset = state.right_label_set[S0];
      S0hl = state.deprels[S0];

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

    int N0 = state.buffer;
    if (N0 < len) {
      N0w = forms[N0]; N0p = postags[N0];
      N0la = state.nr_left_children[N0];
      N0lset = state.left_label_set[N0];

      int N0ld = state.left_most_child[N0];
      if (N0ld >= 0) {
        N0ldw = forms[N0ld]; N0ldp = postags[N0ld]; N0ldl = state.deprels[N0ld];
      }

      int N0l2d = state.left_2nd_most_child[N0];
      if (N0l2d >= 0) {
        N0l2dw = forms[N0l2d]; N0l2dp = postags[N0l2d]; N0l2dl = state.deprels[N0l2d];
      }
    }

    int N1 = state.buffer + 1;
    if (N1 < len) {
      N1w = forms[N1]; N1p = postags[N1];
    }

    int N2 = state.buffer + 2;
    if (N2 < len) {
      N2w = forms[N2]; N2p = postags[N2];
    }

    int S0h = state.heads[state.top0];
    if (S0h >= 0) {
      S0hw = forms[S0h]; S0hp = postags[S0h]; S0h2l = state.deprels[S0h];
    }

    int S0h2 = (S0h >= 0 ? state.heads[S0h] : -1);
    if (S0h2 >= 0) {
      S0h2w = forms[S0h2]; S0h2p = postags[S0h2];
    }

    if (S0 >= 0 && N0 < len) {
      DistS0N0 = Math::binned_1_2_3_4_5_6_10[N0 - S0];
    }
  };

  form_t   S0w, S0ldw, S0rdw, S0l2dw, S0r2dw;
  form_t   N0w, N0ldw,        N0l2dw;
  form_t   N1w;
  form_t   N2w;
  form_t   S0hw;
  form_t   S0h2w;

  postag_t S0p, S0ldp, S0rdp, S0l2dp, S0r2dp;
  postag_t N0p, N0ldp,        N0l2dp;
  postag_t N1p;
  postag_t N2p;
  postag_t S0hp;
  postag_t S0h2p;

  deprel_t      S0ldl, S0rdl, S0l2dl, S0r2dl;
  deprel_t      N0ldl,        N0l2dl;
  deprel_t      S0hl;
  deprel_t      S0h2l;

  int           S0la,  S0ra,  N0la;
  int           S0lset,S0rset,N0lset;
  int           DistS0N0;
};

} //  end for namespace arceager
} //  end for namespace dependencyparser
} //  end for namespace zuopar
#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCEAGER_SCORE_CONTEXT_H__
