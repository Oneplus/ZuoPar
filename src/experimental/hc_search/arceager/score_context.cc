#include "experimental/hc_search/arceager/score_context.h"

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
    DistS0N0 = Math::binned_1_2_3_4_5_6_10[N0 - S0];;
  }
}

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
      tree[hid].push_back(i);
    } else {
      root = hid;
    }
  }

  for (int hid = 0; hid < len; ++ hid) {
    H.push_back( hid );
    H_H.push_back( std::make_pair(hid, hid) );
    H_H_H.push_back( boost::make_tuple(hid, hid, hid) );
    H_H_pH.push_back( boost::make_tuple(hid, hid, hid- 1) );
    H_H_nH.push_back( boost::make_tuple(hid, hid, hid+ 1) );
    H_pH_H_nH.push_back( boost::make_tuple(hid, hid- 1, hid, hid+ 1));
    const std::vector<int>& hnode = tree[hid];
    for (int j = 0; j < hnode.size(); ++ j) {
      int mid = hnode[j];
      H_M.push_back( std::make_pair(hid, mid) );

      // head modifier context group (1)
      pH_H_M_Mn.push_back( boost::make_tuple(hid- 1, hid, mid, mid+ 1));
      pH_H_M.push_back( boost::make_tuple(hid- 1, hid, mid));
      H_M_Mn.push_back( boost::make_tuple(hid, mid, mid+ 1));
      pH_H_Mn.push_back( boost::make_tuple(hid- 1, hid, mid+ 1));
      pH_M_Mn.push_back( boost::make_tuple(hid- 1, mid, mid+ 1));

      // head modifier context group (2)
      H_nH_pM_M.push_back( boost::make_tuple(hid, hid+1, mid-1, mid));
      H_nH_pM.push_back( boost::make_tuple(hid, hid+1, mid));
      nH_pM_M.push_back( boost::make_tuple(hid+1, mid-1, mid));
      H_nH_M.push_back( boost::make_tuple(hid, hid+1, mid));
      H_pM_M.push_back( boost::make_tuple(hid, mid-1, mid));

      H_H_M.push_back( boost::make_tuple(hid, hid, mid) );
      H_M_M.push_back( boost::make_tuple(hid, mid, mid) );
      H_H_M_M.push_back( boost::make_tuple(hid, hid, mid, mid) );
    }

    for (int j = 1; j < hnode.size(); ++ j) {
      int mid = hnode[j- 1], sid = hnode[j];
      consecutive_siblings.push_back( boost::make_tuple(hid, mid, sid) );

      const std::vector<int>& mnode = tree[mid];
      for (int l = 0; l < mnode.size(); ++ l) {
        int gcid = mnode[l];
        outer_sibling_grandchildren.push_back(boost::make_tuple(hid, mid, gcid, sid));
      }

      const std::vector<int>& snode = tree[sid];
      for (int l = 0; l < snode.size(); ++ l) {
        int gcid = snode[l];
        inner_sibling_grandchildren.push_back(boost::make_tuple(hid, mid, sid, gcid));
      }
    }

    for (int j = 2; j < hnode.size(); ++ j) {
      int mid = hnode[j- 2], sid = hnode[j- 1], tid = hnode[j];
      tri_siblings.push_back(boost::make_tuple(hid, mid, sid, tid));
    }
  }

  for (int gid = 0; gid < len; ++ gid) {
    const std::vector<int>& gnode = tree[gid];
    for (int j = 0; j < gnode.size(); ++ j) {
      int hid = gnode[j];
      const std::vector<int>& hnode = tree[hid];
      for (int k = 0; k < hnode.size(); ++ k) {
        int mid = hnode[k];
        grandparents.push_back( boost::make_tuple(gid, hid, mid) );
      }

      for (int k = 1; k < hnode.size(); ++ k) {
        int mid = hnode[k- 1], sid = hnode[k];
        grand_siblings.push_back( boost::make_tuple(gid, hid, mid, sid) );
      }
    }
  }
}

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
