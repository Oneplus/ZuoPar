#include "utils/math/fast_binned.h"
#include "app/depparser/choi2013/score_context.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

ScoreContext::ScoreContext(const State& state):
  S0w(0), S0ldw(0), S0rdw(0), S0l2dw(0), S0r2dw(0), S1w(0),
  N0w(2), N0ldw(0), N0l2dw(0),
  S0hw(0), S0h2w(0), N0hw(0), N0h2w(0),
  S0L2w(0), S0L1w(0), S0R1w(0), S0R2w(0), N0L2w(0), N0L1w(0), N0R1w(0), N0R2w(0),
  S0p(0), S0ldp(0), S0rdp(0), S0l2dp(0), S0r2dp(0), S1p(0),
  N0p(2), N0ldp(0), N0l2dp(0),
  S0hp(0), S0h2p(0), N0hp(0), N0h2p(0),
  S0L2p(0), S0L1p(0), S0R1p(0), S0R2p(0), N0L2p(0), N0L1p(0), N0R1p(0), N0R2p(0),
  S0ldl(0), S0rdl(0), S0l2dl(0), S0r2dl(0), N0ldl(0), N0l2dl(0),
  S0hl(0), S0h2l(0), N0hl(0), N0h2l(0),
  S0la(0), S0ra(0), N0la(0),
  S0lsetl(0), S0lseth(0), S0rsetl(0), S0rseth(0), N0lsetl(0), N0lseth(0),
  DistS0N0(0), S0Feat(0), N0Feat(0),
  S0IsBegin(0), N0IsEnd(0), S0N0Adjacent(0) {
  const std::vector<form_t>& FORM = state.ref->forms;
  const std::vector<postag_t>& POS = state.ref->postags;
  const int* REL = state.deprels;
  int len = state.ref->size();

  int S0 = state.top0;
  if (S0 >= 0) {
    S0w = FORM[S0]; S0p = POS[S0]; S0hl = REL[S0];
    S0la = state.nr_left_children[S0]; S0ra = state.nr_right_children[S0];
    S0lsetl = state.left_label_set_lowbit[S0];
    S0lseth = state.left_label_set_highbit[S0];
    S0rsetl = state.right_label_set_lowbit[S0];
    S0rseth = state.right_label_set_highbit[S0];
    S0Feat = (&state.ref->feats[S0]);

    int S0ld = state.left_most_child[S0];
    if (S0ld >= 0) { S0ldw = FORM[S0ld]; S0ldp = POS[S0ld]; S0ldl = REL[S0ld]; }

    int S0rd = state.right_most_child[S0];
    if (S0rd >= 0) { S0rdw = FORM[S0rd]; S0rdp = POS[S0rd]; S0rdl = REL[S0rd]; }

    int S0l2d = state.left_2nd_most_child[S0];
    if (S0l2d >= 0) { S0l2dw = FORM[S0l2d]; S0l2dp = POS[S0l2d]; S0l2dl = REL[S0l2d]; }

    int S0r2d = state.right_2nd_most_child[S0];
    if (S0r2d >= 0) { S0r2dw = FORM[S0r2d]; S0r2dp = POS[S0r2d]; S0r2dl = REL[S0r2d]; }

    int S0h = state.heads[S0];
    if (S0h >= 0) { S0hw = FORM[S0h]; S0hp = POS[S0h]; S0h2l = REL[S0h]; }

    int S0h2 = (S0h >= 0 ? state.heads[S0h] : -1);
    if (S0h2 >= 0) { S0h2w = FORM[S0h2]; S0h2p = POS[S0h2]; }

    int S0L2 = S0- 1;
    if (S0L2 >= 0)  { S0L2w = FORM[S0L2]; S0L2p = POS[S0L2]; } else { S0L2w = S0L2p = 1; }

    int S0L1 = S0- 1;
    if (S0L1 >= 0)  { S0L1w = FORM[S0L1]; S0L1p = POS[S0L1]; } else { S0L1w = S0L1p = 1; }

    int S0R1 = S0+ 1;
    if (S0R1 < len) { S0R1w = FORM[S0R1]; S0R1p = POS[S0R1]; } else { S0R1w = S0R1p = 2; }

    int S0R2 = S0+ 2;
    if (S0R2 < len) { S0R2w = FORM[S0R2]; S0R2p = POS[S0R2]; } else { S0R2w = S0R2p = 2; }

    int S1 = state.top1; if (S1 >= 0) { S1w = FORM[S1]; S1p = POS[S1]; }
  }

  S0IsBegin = (S0 == 0 ? 1: (S0 == -1 ? 2: 3));

  int N0 = state.buffer;
  if (N0 < len) {
    N0w = FORM[N0]; N0p = POS[N0]; N0hl = REL[N0];
    N0la = state.nr_left_children[N0];
    N0lsetl = state.left_label_set_lowbit[N0];
    N0lseth = state.left_label_set_highbit[N0];
    N0Feat = (&state.ref->feats[N0]);

    int N0ld = state.left_most_child[N0];
    if (N0ld >= 0) { N0ldw = FORM[N0ld]; N0ldp = POS[N0ld]; N0ldl = REL[N0ld]; }

    int N0l2d = state.left_2nd_most_child[N0];
    if (N0l2d >= 0) { N0l2dw = FORM[N0l2d]; N0l2dp = POS[N0l2d]; N0l2dl = REL[N0l2d]; }

    int N0h = state.heads[N0];
    if (N0h >= 0) { N0hw = FORM[N0h]; N0hp = POS[N0h]; N0h2l = REL[N0h]; }

    int N0h2 = (N0h >= 0 ? state.heads[N0h] : -1);
    if (N0h2 >= 0) { N0h2w = FORM[N0h2]; N0h2p = POS[N0h2]; }

    int N0L2 = N0- 2;
    if (N0L2 >= 0)  { N0L2w = FORM[N0L2]; N0L2p = POS[N0L2]; } else { N0L2w = N0L2p = 1; }

    int N0L1 = N0- 1;
    if (N0L1 >= 0)  { N0L1w = FORM[N0L1]; N0L1p = POS[N0L1]; } else { N0L1w = N0L1p = 1; }

    int N0R1 = N0+ 1;
    if (N0R1 < len) { N0R1w = FORM[N0R1]; N0R1p = POS[N0R1]; } else { N0R1w = N0R1p = 2; }

    int N0R2 = N0+ 2;
    if (N0R2 < len) { N0R2p = FORM[N0R2]; N0R2p = POS[N0R2]; } else { N0R2w = N0R2p = 2; }
  }

  N0IsEnd = (N0+ 1 == len? 1: 2);

  if (S0 >= 0 && N0 < len) {
    DistS0N0 = Math::binned_1_2_3_4_5_6_10[N0 - S0];
    S0N0Adjacent = (S0 + 1 == N0 ? 1: 2);
  } else { S0N0Adjacent = 3; }
};


} //  end for namespace choi2013
} //  end for namespace dependencyparser
} //  end for namespace zuopar
