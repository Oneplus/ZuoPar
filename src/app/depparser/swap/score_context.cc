#include "app/depparser/swap/score_context.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

ScoreContext::ScoreContext(const State& state):
  S0w(0), S0ldw(0), S0l2dw(0), S0rdw(0), S0r2dw(0), S0ldl(0), S0l2dl(0),
  S0ldldw(0), S0ldldp(0), S0ldldl(0),
  S0p(0), S0ldp(0), S0l2dp(0), S0rdp(0), S0r2dp(0), S0rdl(0), S0r2dl(0),
  S0rdrdw(0), S0rdrdp(0), S0rdrdl(0),
  S0la(0), S0ra(0),
  S0L2w(0), S0L1w(0), S0R1w(0), S0R2w(0), S0L2p(0), S0L1p(0), S0R1p(0), S0R2p(0),

  S1w(0), S1ldw(0), S1l2dw(0), S1rdw(0), S1r2dw(0), S1ldl(0), S1l2dl(0),
  S1ldldw(0), S1ldldp(0), S1ldldl(0),
  S1p(0), S1ldp(0), S1l2dp(0), S1rdp(0), S1r2dp(0), S1rdl(0), S1r2dl(0),
  S1rdrdw(0), S1rdrdp(0), S1rdrdl(0),
  S1la(0), S1ra(0),
  S1L2w(0), S1L1w(0), S1R1w(0), S1R2w(0), S1L2p(0), S1L1p(0), S1R1p(0), S1R2p(0),

  S0lsetl(0), S0lseth(0), S0rsetl(0), S0rseth(0),
  S1lsetl(0), S1lseth(0), S1rsetl(0), S1rseth(0),
  DistS0S1(0), DistS0N0(0), S1Feat(0), S0Feat(0), N0Feat(0),

  N0w(2), N0p(2), N1w(0), N1p(0),
  N0L2w(0), N0L1w(0), N0R1w(0), N0R2w(0), N0L2p(0), N0L1p(0), N0R1p(0), N0R2p(0) {
  const std::vector<form_t>& FORM = state.ref->forms;
  const std::vector<postag_t>& POS = state.ref->postags;
  const int* REL = state.deprels;
  int len = state.ref->size();

  int S0 = state.top0;
  if (S0 >= 0) {
    S0w = FORM[S0]; S0p = POS[S0];
    S0la = state.nr_left_children[S0]; S0ra = state.nr_right_children[S0];
    S0lsetl = state.left_label_set_lowbit[S0];  S0lseth = state.left_label_set_highbit[S0];
    S0rsetl = state.right_label_set_lowbit[S0]; S0rseth = state.right_label_set_highbit[S0];
    S0Feat = (&state.ref->feats[S0]);

    int S0ld = state.left_most_child[S0];
    if (S0ld >= 0) {
      S0ldw = FORM[S0ld]; S0ldp = POS[S0ld]; S0ldl = REL[S0ld];
      int S0ldld = state.left_most_child[S0ld];
      if (S0ldld >= 0) { S0ldldw = FORM[S0ldld]; S0ldldp = POS[S0ldld]; S0ldldl = REL[S0ldld]; }
    }

    int S0rd = state.right_most_child[S0];
    if (S0rd >= 0) {
      S0rdw = FORM[S0rd]; S0rdp = POS[S0rd]; S0rdl = REL[S0rd];
      int S0rdrd = state.right_most_child[S0rd];
      if (S0rdrd >= 0) { S0rdrdw = FORM[S0rdrd]; S0rdrdp = POS[S0rdrd]; S0rdrdl = REL[S0rdrd]; }
    }

    int S0l2d = state.left_2nd_most_child[S0];
    if (S0l2d >= 0) { S0l2dw = FORM[S0l2d]; S0l2dp = POS[S0l2d]; S0l2dl = REL[S0l2d]; }

    int S0r2d = state.right_2nd_most_child[S0];
    if (S0r2d >= 0) { S0r2dw = FORM[S0r2d]; S0r2dp = POS[S0r2d]; S0r2dl = REL[S0r2d]; }

    int S0L2 = S0- 2;
    if (S0L2 >= 0)  { S0L2w = FORM[S0L2]; S0L2p = POS[S0L2]; } else { S0L2w = S0L2p = 1; }

    int S0L1 = S0- 1;
    if (S0L1 >= 0)  { S0L1w = FORM[S0L1]; S0L1p = POS[S0L1]; } else { S0L1w = S0L1p = 1; }

    int S0R1 = S0+ 1;
    if (S0R1 < len) { S0R1w = FORM[S0R1]; S0R1p = POS[S0R1]; } else { S0R1w = S0R1p = 2; }

    int S0R2 = S0+ 2;
    if (S0R2 < len) { S0R2w = FORM[S0R2]; S0R2p = POS[S0R2]; } else { S0R2w = S0R2p = 2; }
  }
  S0IsEnd = (S0+1 == len? 1: 2);

  int S1 = state.top1;
  if (S1 >= 0) {
    S1w = FORM[S1]; S1p = POS[S1];
    S1la = state.nr_left_children[S1]; S1ra = state.nr_right_children[S1];
    S1lsetl = state.left_label_set_lowbit[S1];  S1lseth = state.left_label_set_highbit[S1];
    S1rsetl = state.right_label_set_lowbit[S1]; S1rseth = state.right_label_set_highbit[S1];
    S1Feat = (&state.ref->feats[S1]);

    int S1ld = state.left_most_child[S1];
    if (S1ld >= 0) {
      S1ldw = FORM[S1ld]; S1ldp = POS[S1ld]; S1ldl = REL[S1ld];
      int S1ldld = state.left_most_child[S1ld];
      if (S1ldld >= 0) { S1ldldw = FORM[S1ldld]; S1ldldp = POS[S1ldld]; S1ldldl = REL[S1ldld]; }
    }

    int S1rd = state.right_most_child[S1];
    if (S1rd >= 0) {
      S1rdw = FORM[S1rd]; S1rdp = POS[S1rd]; S1rdl = REL[S1rd];
      int S1rdrd = state.right_most_child[S1rd];
      if (S1rdrd >= 0) { S1rdrdw = FORM[S1rdrd]; S1rdrdp = POS[S1rdrd]; S1rdrdl = REL[S1rdrd]; }
    }

    int S1l2d = state.left_2nd_most_child[S1];
    if (S1l2d >= 0) { S1l2dw = FORM[S1l2d]; S1l2dp = POS[S1l2d]; S1l2dl= REL[S1l2d]; }

    int S1r2d = state.right_2nd_most_child[S1];
    if (S1r2d >= 0) { S1r2dw = FORM[S1r2d]; S1r2dp = POS[S1r2d]; S1r2dl= REL[S1r2d]; }

    int S1L2 = S1- 2;
    if (S1L2 >= 0)  { S1L2w = FORM[S1L2]; S1L2p = POS[S1L2]; } else { S1L2w = S1L2p = 1; }

    int S1L1 = S1- 1;
    if (S1L1 >= 0)  { S1L1w = FORM[S1L1]; S1L1p = POS[S1L1]; } else { S1L1w = S1L1p = 1; }

    int S1R1 = S1+ 1;
    if (S1R1 < len) { S1R1w = FORM[S1R1]; S1R1p = POS[S1R1]; } else { S1R1w = S1R1p = 2; }

    int S1R2 = S1+ 2;
    if (S1R2 < len) { S1R2w = FORM[S1R2]; S1R2p = POS[S1R2]; } else { S1R2w = S1R2p = 2; }
  }
  S1IsBegin = (S1 == 0 ? 1: (S1 == -1? 2: 3));

  if (S0 >= 0 && S1 >= 0) {
    DistS0S1 = (S0 > S1 ?
        Math::binned_1_2_3_4_5_6_10[S0 - S1]:
        -Math::binned_1_2_3_4_5_6_10[S1 - S0]);
    S1S0Adjacent = (S1 + 1 == S0? 1: (S0+ 1 == S1? 2: 3));
  } else { S1S0Adjacent = 4; }

  int N0 = state.front0;
  if (N0 >= 0) {
    N0w = FORM[N0]; N0p = POS[N0];
    N0Feat = (&state.ref->feats[N0]);

    int N0L2 = N0- 2;
    if (N0L2 >= 0)  { N0L2w = FORM[N0L2]; N0L2p = POS[N0L2]; } else { N0L2w = N0L2p = 1; }

    int N0L1 = N0- 1;
    if (N0L1 >= 0)  { N0L1w = FORM[N0L1]; N0L1p = POS[N0L1]; } else { N0L1w = N0L1p = 1; }

    int N0R1 = N0+ 1;
    if (N0R1 < len) { N0R1w = FORM[N0R1]; N0R1p = POS[N0R1]; } else { N0R1w = N0R1p = 2; }

    int N0R2 = N0+ 2;
    if (N0R2 < len) { N0R2w = FORM[N0R2]; N0R2p = POS[N0R2]; } else { N0R2w = N0R2p = 2; }
  }

  int N1 = state.front1;
  if (N1 >= 0) {
    N1w = FORM[N1]; N1p = POS[N1];
  }

  if (S0 >= 0 && N0 >= 0) {
    DistS0N0 = (N0 > S0 ?
        Math::binned_1_2_3_4_5_6_10[N0 - S0]:
        -Math::binned_1_2_3_4_5_6_10[S0 - N0]);
    S0N0Adjacent = (S0 + 1 == N0? 1: (N0+ 1 == S0? 2: 3));
  } else { S0N0Adjacent = 4; }

  N0IsEnd = (N0+ 1 == len? 1: 2);
};

} // namespace swap
} // namespace dependencyparser
} // namespace zuopar
