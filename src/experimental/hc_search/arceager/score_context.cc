#include "experimental/hc_search/arceager/score_context.h"
#include "experimental/hc_search/arceager/knowledge.h"
#include "utils/math/fast_binned.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

HeuristicScoreContext::HeuristicScoreContext(const State& state)
  : S0w(0), S0ldw(0), S0rdw(0), S0l2dw(0), S0r2dw(0),
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
}

#define __M boost::make_tuple

CostScoreContext::CostScoreContext(const State& state)
  : forms(state.ref->forms),
  postags(state.ref->postags),
  deprels(state.deprels) {
  len = state.ref->size();
  int root = -1;
  std::vector< std::vector<int> > tree(len);

  for (int i = 0; i < len; ++ i) {
    int hid = state.heads[i];
    if (hid != -1) {
      if (!extract_punctuation && PUNC_POS.find(postags[i]) != PUNC_POS.end()) {
        continue;
      }
      tree[hid].push_back(i);
    }
    else { root = i; }
  }

  // Calculate right branch.
  RB = 0; int now = root;
  while (tree[now].size() != 0) {
    ++ RB;
    now = tree[now].back();
  }
  RB = RB * 255 / len;
  RB = Math::binned_1_2_3_4_5_6_10[RB]+ 1;

  RANK = state.top0;
  SCORE = int(state.score * 100)+ 1;

  span_length.resize(len, 0);
  non_punctuation_span_length(root, postags, tree, span_length);
  /*for (int i = 0; i < len; ++ i) {
    span_length[i] = Math::binned_1_2_3_4_5_6_10[span_length[i]];
  }*/

  nr_children.resize(len, 0);
  nr_left_children.resize(len, 0);
  nr_right_children.resize(len, 0);
  label_set.resize(len, 0);
  left_label_set.resize(len, 0);
  right_label_set.resize(len, 0);

  for (int i = 0; i < len; ++ i) {
    for (int c: tree[i]) {
      if (PUNC_POS.find(postags[c]) == PUNC_POS.end()) {
        //! Non-punctuation.
        ++ nr_children[i];
        label_set[i] |= deprels[c];
        if (c < i) {
          ++ nr_left_children[i];
          left_label_set[i] |= deprels[c];
        } else {
          ++ nr_right_children[i];
          right_label_set[i] |= deprels[c];
        }
      }
    }
    //nr_children[i] = Math::binned_1_2_3_4_5_6_10[nr_children[i]];
    //nr_left_children[i] = Math::binned_1_2_3_4_5_6_10[nr_left_children[i]];
    //nr_right_children[i] = Math::binned_1_2_3_4_5_6_10[nr_right_children[i]];
  }

  for (int i = 0; i < len; ++ i) {
    if (ADP_POS.find(postags[i]) != ADP_POS.end()) {
      int hid = state.heads[i];
      for (int mid: tree[i]) {
        int Dir  = ((hid < i) << 1 | (i < mid) << 2);
        PP_H_M.push_back(__M(hid, mid));
        PP_P_H_M.push_back(__M(i, hid, mid));

        PP_H_M_Dir.push_back(__M(hid, mid, Dir));
        PP_P_H_M_Dir.push_back(__M(i, hid, mid, Dir));
      }
    }
  }

  for (int hid = 0; hid < len; ++ hid) {
    // Begin singular
    H.push_back(hid);
    H_H.push_back( __M(hid, hid) );
    H_H_H.push_back( __M(hid, hid, hid) );
    H_pH_H.push_back( __M(hid, hid- 1, hid) );
    H_nH_H.push_back( __M(hid, hid+ 1, hid) );
    pH_H_nH_H.push_back( __M(hid- 1, hid, hid+ 1, hid) );
    // End singular

    // Begin first-order
    const std::vector<int>& hnode = tree[hid];
    for (int j = 0; j < hnode.size(); ++ j) {
      int mid = hnode[j];
      int Rel = state.deprels[mid];
      int Dir  = (hid < mid? 0: 1);
      int Dist = (hid < mid?
          Math::binned_1_2_3_4_5_6_10[mid- hid]:
          Math::binned_1_2_3_4_5_6_10[hid- mid]);

      // relation
      H_M.push_back( __M(hid, mid) );
      H_H_M.push_back( __M(hid, hid, mid));
      H_M_M.push_back( __M(hid, mid, mid));
      H_H_M_M.push_back( __M(hid, hid, mid, mid));

      H_M_Rel.push_back( __M(hid, mid, Rel) );
      H_H_M_Rel.push_back( __M(hid, hid, mid, Rel));
      H_M_M_Rel.push_back( __M(hid, mid, mid, Rel));
      H_H_M_M_Rel.push_back( __M(hid, hid, mid, mid, Rel));

      H_M_Dir.push_back( __M(hid, mid, Dir) );
      H_H_M_Dir.push_back( __M(hid, hid, mid, Dir) );
      H_M_M_Dir.push_back( __M(hid, mid, mid, Dir) );
      H_H_M_M_Dir.push_back( __M(hid, hid, mid, mid, Dir) );

      H_M_Dist.push_back( __M(hid, mid, Dist) );
      H_M_M_Dist.push_back( __M(hid, mid, mid, Dist) );
      H_H_M_Dist.push_back( __M(hid, hid, mid, Dist) );
      H_H_M_M_Dist.push_back( __M(hid, hid, mid, mid, Dist) );

      // context
      pH_H_M_Mn.push_back(__M(hid- 1, hid, mid, mid+ 1));
      pH_H_M.push_back(__M(hid- 1, hid, mid));
      H_M_Mn.push_back(__M(hid, mid, mid+ 1));
      pH_H_Mn.push_back(__M(hid- 1, hid, mid+ 1));
      pH_M_Mn.push_back(__M(hid- 1, mid, mid+ 1));
      H_nH_pM_M.push_back(__M(hid, hid+1, mid-1, mid));
      H_nH_pM.push_back(__M(hid, hid+1, mid));
      nH_pM_M.push_back(__M(hid+1, mid-1, mid));
      H_nH_M.push_back(__M(hid, hid+1, mid));
      H_pM_M.push_back(__M(hid, mid-1, mid));
    }
    // End first order

    for (int j = 1; j < hnode.size(); ++ j) {
      int sid = hnode[j- 1], mid = hnode[j];
      if ((hid < sid) != (hid < mid)) { continue; }

      int Rel = (state.deprels[mid] << 8 | state.deprels[sid]);
      int Dir = (((hid < sid) << 1) | ((hid < mid) << 2));

      H_S_M.push_back( __M(hid, sid, mid) );
      H_S_M_Rel.push_back( __M(hid, sid, mid, Rel) );
      H_S_M_Dir.push_back( __M(hid, sid, mid, Dir) );

      S_M.push_back( __M(sid, mid) );
      S_M_Rel.push_back( __M(sid, mid, Rel) );
      S_M_Dir.push_back( __M(sid, mid, Dir) );

      pH_H_S_M.push_back( __M(hid- 1, hid, sid, mid) );
      H_nH_S_M.push_back( __M(hid, hid+ 1, sid, mid) );
      H_pS_S_M.push_back( __M(hid, sid- 1, sid, mid) );
      H_S_nS_M.push_back( __M(hid, sid, sid+ 1, mid) );
      H_S_pM_M.push_back( __M(hid, sid, mid- 1, mid) );
      H_S_M_nM.push_back( __M(hid, sid, mid, mid+ 1) );

      const std::vector<int>& snode = tree[sid];
      for (int l = 0; l < snode.size(); ++ l) {
        int gcid = snode[l];
        int Rel = (((state.deprels[mid] << 8) | state.deprels[sid]) << 8)| state.deprels[gcid];
        int Dir = (((hid < mid) << 1) | ((hid < sid) << 2) | ((sid < gcid) << 3));
        H_S_GC_M.push_back(__M(hid, sid, gcid, mid));
        H_S_GC_M_Rel.push_back(__M(hid, sid, gcid, mid, Rel));
        H_S_GC_M_Dir.push_back(__M(hid, sid, gcid, mid, Dir));
      }

      const std::vector<int>& mnode = tree[mid];
      for (int l = 0; l < mnode.size(); ++ l) {
        int gcid = mnode[l];
        int Rel = (((state.deprels[mid] << 8) | state.deprels[sid]) << 8) |state.deprels[gcid];
        int Dir = (((hid < mid) << 1) | ((hid < sid) << 2) | ((mid < gcid) << 3));
        H_S_M_GC.push_back(__M(hid, sid, mid, gcid));
        H_S_M_GC_Rel.push_back(__M(hid, sid, mid, gcid, Rel));
        H_S_M_GC_Dir.push_back(__M(hid, sid, mid, gcid, Dir));
      }
    }

    for (int j = 2; j < hnode.size(); ++ j) {
      int mid = hnode[j- 2], sid = hnode[j- 1], tid = hnode[j];
      if ((mid < hid) != (sid < hid) || (mid < hid) != (tid < hid)) { continue; }
      int Rel = ((state.deprels[mid]<<8 | state.deprels[sid]) << 8 | state.deprels[tid]);
      int Dir = ((hid < mid) << 1 | (hid < sid) << 2 | (hid < tid) << 3);
      H_M_S_T.push_back( __M(hid, mid, sid, tid) );
      H_M_S_T_Rel.push_back( __M(hid, mid, sid, tid, Rel) );
      H_M_S_T_Dir.push_back( __M(hid, mid, sid, tid, Dir) );

      H_M_T.push_back( __M(hid, mid, tid) );
      H_M_T_Rel.push_back( __M(hid, mid, tid, Rel) );
      H_M_T_Dir.push_back( __M(hid, mid, tid, Dir) );
    }
  }

  for (int mid = 1; mid < len; ++ mid) {
    int mid1 = mid - 1;
    int hid = state.heads[mid];
    int hid1 = state.heads[mid1];
    int Dir = ((hid < mid) << 1 | (hid1 < mid1) << 2);
    H1_H_M1_M.push_back( __M(hid1, hid, mid1, mid) );
    H1_H_M1_M_Dir.push_back( __M(hid1, hid, mid1, mid, Dir) );
  }

  for (int gid = 0; gid < len; ++ gid) {
    const std::vector<int>& gnode = tree[gid];
    for (int j = 0; j < gnode.size(); ++ j) {
      int hid = gnode[j];

      const std::vector<int>& hnode = tree[hid];
      for (int k = 0; k < hnode.size(); ++ k) {
        int mid = hnode[k];
        int Rel = (state.deprels[hid]<<8 | state.deprels[mid]);
        int Dir = ((gid < hid) << 1 | (hid < mid) << 2);

        G_P_M.push_back( __M(gid, hid, mid) );
        G_P_M_Rel.push_back( __M(gid, hid, mid, Rel) );
        G_P_M_Dir.push_back( __M(gid, hid, mid, Dir) );

        G_M.push_back( __M(gid, mid) );
        G_M_Rel.push_back( __M(gid, mid, state.deprels[mid]) );
        G_M_Dir.push_back( __M(gid, mid, Dir) );

        pG_G_P_M.push_back( __M(gid- 1, gid, hid, mid) );
        G_nG_P_M.push_back( __M(gid, gid+ 1, hid, mid) );
        G_pP_P_M.push_back( __M(gid, hid- 1, hid, mid) );
        G_P_nP_M.push_back( __M(gid, hid, hid+ 1, mid) );
        G_P_pM_M.push_back( __M(gid, hid, mid- 1, mid) );
        G_P_M_nM.push_back( __M(gid, hid, mid, mid+ 1) );

        const std::vector<int>& mnode = tree[mid];
        for (int l = 0; l < mnode.size(); ++ l) {
          int cid = mnode[l];
          int Rel = ((state.deprels[hid]<<8| state.deprels[mid])<< 8| state.deprels[cid]);
          int Dir = (((gid < hid) << 1) | ((hid < mid) << 2) | ((mid < cid) << 3));

          G_P_M_C.push_back( __M(gid, hid, mid, cid) );
          G_P_M_C_Rel.push_back( __M(gid, hid, mid, cid, Rel) );
          G_P_M_C_Dir.push_back( __M(gid, hid, mid, cid, Dir) );

          Rel = (state.deprels[hid]<< 8| state.deprels[cid]);
          G_P_C.push_back( __M(gid, hid, cid) );
          G_P_C_Rel.push_back( __M(gid, hid, cid, Rel) );
          G_P_C_Dir.push_back( __M(gid, hid, cid, Dir) );

          Rel = (state.deprels[mid]<<8| state.deprels[cid]);
          G_M_C.push_back( __M(gid, mid, cid) );
          G_M_C_Rel.push_back( __M(gid, mid, cid, Rel) );
          G_M_C_Dir.push_back( __M(gid, mid, cid, Dir) );

          G_C.push_back( __M(gid, cid) );
          G_C_Rel.push_back( __M(gid, cid, state.deprels[cid]) );
          G_C_Dir.push_back( __M(gid, cid, Dir) );
        }
      }

      for (int k = 1; k < hnode.size(); ++ k) {
        int mid = hnode[k- 1], sid = hnode[k];
        if ((hid < mid) != (hid < sid)) { continue; }
        int Rel = ((state.deprels[hid]<< 8 | state.deprels[mid]) << 8| state.deprels[sid]);
        int Dir = (((gid < hid) << 1) | ((hid < mid) << 2) | ((hid < sid) << 3));

        G_P_M_S.push_back( __M(gid, hid, mid, sid) );
        G_P_M_S_Rel.push_back( __M(gid, hid, mid, sid, Rel) );
        G_P_M_S_Dir.push_back( __M(gid, hid, mid, sid, Dir) );

        Rel = state.deprels[mid]<<8 | state.deprels[sid];
        G_M_S.push_back( __M(gid, mid, sid) );
        G_M_S_Rel.push_back( __M(gid, mid, sid, Rel) );
        G_M_S_Dir.push_back( __M(gid, mid, sid, Dir) );
      }
    }
  }
}

int CostScoreContext::non_punctuation_span_length(int now,
    const std::vector<postag_t>& postags,
    const std::vector<std::vector<int> >& tree,
    std::vector<int>& result) {
  result[now] = 0;
  const std::vector<int>& node = tree[now];
  for (int child: node) {
    result[now] += non_punctuation_span_length(child, postags, tree, result);
  }
  if (PUNC_POS.find(postags[now]) == PUNC_POS.end()) {
    // Non-punctuation
    result[now] += 1;
  }
  return result[now];
}

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
