#include "experimental/hc_search/score_context.h"
#include "experimental/hc_search/knowledge.h"
#include "utils/math/fast_binned.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace CStep {

#define __M boost::make_tuple
#define __R(name) (name).reserve(len)

ScoreContext::ScoreContext(int   _len,
    const std::vector<form_t>&   _forms,
    const std::vector<postag_t>& _postags,
    const std::vector<int>&      _heads,
    const std::vector<int>&      _deprels)
  : len(_len), forms(_forms), postags(_postags), heads(_heads), deprels(_deprels) {
  int root = 0;
  std::vector< std::vector<int> > tree(len);
  for (int i = 0; i < len; ++ i) {
    int hid = heads[i];
    if (hid != -1) { tree[hid].push_back(i); }
    else { root = i; }
  }

  // Calculate right branch.
  RB = 0; int now = root;
  while (tree[now].size() != 0) { ++ RB; now = tree[now].back(); }
  RB = RB * 255 / len;
  RB = Math::binned_1_2_3_4_5_6_10[RB]+ 1;

  span_length.resize(len, 0);
  non_punctuation_span_length(root, postags, tree, span_length);

  nr_children.resize(len, 0);
  nr_left_children.resize(len, 0);
  nr_right_children.resize(len, 0);
  label_set.resize(len, 0);
  left_label_set.resize(len, 0);
  right_label_set.resize(len, 0);

  for (auto i = 1; i < len; ++ i) {
    for (auto& c: tree[i]) {
      if (PUNC_POS.find(postags[c]) == PUNC_POS.end()) {
        //! Non-punctuation.
        ++ nr_children[i];
        label_set[i] |= deprels[c];
        if (c < i) { ++ nr_left_children[i];  left_label_set[i] |= deprels[c];  }
        else       { ++ nr_right_children[i]; right_label_set[i] |= deprels[c]; }
      }
    }
  }

  __R( PP_H_M ); __R( PP_H_M_Dir ); __R( PP_P_H_M ); __R( PP_P_H_M_Dir );
  for (int i = 1; i < len; ++ i) {
    if (ADP_POS.find(postags[i]) != ADP_POS.end()) {
      int hid = heads[i];
      for (int mid: tree[i]) {
        int Dir  = ((hid < i) << 1 | (i < mid) << 2);
        PP_H_M.push_back(__M(hid, mid));
        PP_P_H_M.push_back(__M(i, hid, mid));

        PP_H_M_Dir.push_back(__M(hid, mid, Dir));
        PP_P_H_M_Dir.push_back(__M(i, hid, mid, Dir));
      }
    }
  }

  __R( H_H ); __R( H_H_H ); __R( H_pH_H ); __R( H_nH_H ); __R( pH_H_nH_H );
  __R( H_M );     __R( H_H_M );     __R( H_M_M );     __R( H_H_M_M );
  __R( H_M_Rel ); __R( H_H_M_Rel ); __R( H_M_M_Rel ); __R( H_H_M_M_Rel );
  __R( H_M_Dist); __R( H_H_M_Dist); __R( H_M_M_Dist); __R( H_H_M_M_Dist);
  __R( H_M_Dir ); __R( H_H_M_Dir ); __R( H_M_M_Dir ); __R( H_H_M_M_Dir );
  __R( pH_H_M_Mn ); __R( pH_H_Mn ); __R( pH_M_Mn );
  __R( H_M_Mn );  __R( pH_H_M );    __R( H_pM_M );

  for (int hid = 1; hid < len; ++ hid) {
    // Begin singular
    H_H.push_back( __M(hid, hid) );
    H_H_H.push_back( __M(hid, hid, hid) );
    if (hid > 0)      H_pH_H.push_back( __M(hid, hid- 1, hid) );
    if (hid+ 1 < len) H_nH_H.push_back( __M(hid, hid+ 1, hid) );
    if (hid> 0 && hid+ 1 < len) pH_H_nH_H.push_back( __M(hid- 1, hid, hid+ 1, hid) );
    // End singular

    // Begin first-order
    const std::vector<int>& hnode = tree[hid];
    for (int mid: hnode) {
      int Rel = deprels[mid];
      int Dir  = (hid < mid? 0: 1);
      int Dist = (hid < mid?
          -Math::binned_1_2_3_4_5_6_10[mid- hid]:
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
      if (hid > 0 && mid+ 1 < len) {
        pH_H_M_Mn.push_back(__M(hid- 1, hid, mid, mid+ 1));
        pH_H_Mn.push_back(__M(hid- 1, hid, mid+ 1));
        pH_M_Mn.push_back(__M(hid- 1, mid, mid+ 1));
      }
      if (hid+ 1 < len && mid > 0) {
        H_nH_pM_M.push_back(__M(hid, hid+1, mid-1, mid));
        nH_pM_M.push_back(__M(hid+1, mid-1, mid));
      }
      if (hid+ 1 < len) {
        H_nH_pM.push_back(__M(hid, hid+1, mid));
        H_nH_M.push_back(__M(hid, hid+1, mid));
      }
      if (mid+ 1 < len) H_M_Mn.push_back(__M(hid, mid, mid+ 1));
      if (hid > 0)      pH_H_M.push_back(__M(hid- 1, hid, mid));
      if (mid > 0)      H_pM_M.push_back(__M(hid, mid-1, mid));
    }
    // End first order

    for (int j = 1; j < hnode.size(); ++ j) {
      int sid = hnode[j- 1], mid = hnode[j];
      if ((sid - hid) * (mid- hid) < 0) { continue; }

      int Rel = (deprels[mid] << 8 | deprels[sid]);
      int Dir = (((hid < sid) << 1) | ((hid < mid) << 2));

      H_S_M.push_back( __M(hid, sid, mid) );
      H_S_M_Rel.push_back( __M(hid, sid, mid, Rel) );
      H_S_M_Dir.push_back( __M(hid, sid, mid, Dir) );

      int Dist = Math::binned_1_2_3_4_5_6_10[mid- sid];
      S_M.push_back( __M(sid, mid) );
      S_M_Rel.push_back( __M(sid, mid, Rel) );
      S_M_Dist.push_back( __M(sid, mid, Dist) );

      if (hid > 0)      pH_H_S_M.push_back( __M(hid- 1, hid, sid, mid) );
      if (hid+ 1 < len) H_nH_S_M.push_back( __M(hid, hid+ 1, sid, mid) );
      if (sid > 0)      H_pS_S_M.push_back( __M(hid, sid- 1, sid, mid) );
      if (sid+ 1 < len) H_S_nS_M.push_back( __M(hid, sid, sid+ 1, mid) );
      if (mid > 0)      H_S_pM_M.push_back( __M(hid, sid, mid- 1, mid) );
      if (mid+ 1 < len) H_S_M_nM.push_back( __M(hid, sid, mid, mid+ 1) );

      const std::vector<int>& snode = tree[sid];
      for (int gcid: snode) {
        int Rel = (((deprels[mid] << 8) | deprels[sid]) << 8)| deprels[gcid];
        int Dir = (((hid < sid) << 1) | ((sid < gcid) << 2));
        H_S_GC_M.push_back(__M(hid, sid, gcid, mid));
        H_S_GC_M_Rel.push_back(__M(hid, sid, gcid, mid, Rel));
        H_S_GC_M_Dir.push_back(__M(hid, sid, gcid, mid, Dir));
      }

      const std::vector<int>& mnode = tree[mid];
      for (int gcid: mnode) {
        int Rel = (((deprels[mid] << 8) | deprels[sid]) << 8) |deprels[gcid];
        int Dir = (((hid < mid) << 1) | ((mid < gcid) << 2));
        H_S_M_GC.push_back(__M(hid, sid, mid, gcid));
        H_S_M_GC_Rel.push_back(__M(hid, sid, mid, gcid, Rel));
        H_S_M_GC_Dir.push_back(__M(hid, sid, mid, gcid, Dir));
      }
    }

    for (int j = 2; j < hnode.size(); ++ j) {
      int mid = hnode[j- 2], sid = hnode[j- 1], tid = hnode[j];
      if ((mid - hid) * (tid- hid) < 0) { continue; }
      int Rel = ((deprels[mid]<<8 | deprels[sid]) << 8 | deprels[tid]);
      int Dir = (hid < sid);
      H_M_S_T.push_back( __M(hid, mid, sid, tid) );
      H_M_S_T_Rel.push_back( __M(hid, mid, sid, tid, Rel) );
      H_M_S_T_Dir.push_back( __M(hid, mid, sid, tid, Dir) );

      H_M_T.push_back( __M(hid, mid, tid) );
      H_M_T_Rel.push_back( __M(hid, mid, tid, Rel) );
      H_M_T_Dir.push_back( __M(hid, mid, tid, Dir) );

      M_S_T.push_back( __M(mid, sid, tid) );
      M_S_T_Rel.push_back( __M(mid, sid, tid, Rel) );
      M_S_T_Dir.push_back( __M(mid, sid, tid, Dir) );
    }
  }

  __R( H1_H_M1_M ); __R( H1_H_M1_M_Dir );
  for (int mid = 1; mid < len; ++ mid) {
    int mid1 = mid - 1;
    int hid = heads[mid];
    int hid1 = heads[mid1];
    if (hid == -1 || hid1 == -1) { continue; }
    int Dir = ((hid < mid) << 1 | (hid1 < mid1) << 2);
    H1_H_M1_M.push_back( __M(hid1, hid, mid1, mid) );
    H1_H_M1_M_Dir.push_back( __M(hid1, hid, mid1, mid, Dir) );
  }

  for (int gid = 1; gid < len; ++ gid) {
    const std::vector<int>& gnode = tree[gid];
    for (int hid: gnode) {
      const std::vector<int>& hnode = tree[hid];
      for (int mid: hnode) {
        int Rel = (deprels[hid]<<8 | deprels[mid]);
        int Dir = ((gid < hid) << 1 | (hid < mid) << 2);

        G_P_M.push_back( __M(gid, hid, mid) );
        G_P_M_Rel.push_back( __M(gid, hid, mid, Rel) );
        G_P_M_Dir.push_back( __M(gid, hid, mid, Dir) );

        G_M.push_back( __M(gid, mid) );
        G_M_Rel.push_back( __M(gid, mid, deprels[mid]) );
        G_M_Dir.push_back( __M(gid, mid, Dir) );

        if (gid > 0)       pG_G_P_M.push_back( __M(gid- 1, gid, hid, mid) );
        if (gid+ 1 < len)  G_nG_P_M.push_back( __M(gid, gid+ 1, hid, mid) );
        if (hid > 0)       G_pP_P_M.push_back( __M(gid, hid- 1, hid, mid) );
        if (hid+ 1 < len)  G_P_nP_M.push_back( __M(gid, hid, hid+ 1, mid) );
        if (mid > 0)       G_P_pM_M.push_back( __M(gid, hid, mid- 1, mid) );
        if (mid+ 1 < len)  G_P_M_nM.push_back( __M(gid, hid, mid, mid+ 1) );

        const std::vector<int>& mnode = tree[mid];
        for (int cid: mnode) {
          int Rel = ((deprels[hid]<<8| deprels[mid])<< 8| deprels[cid]);
          int Dir = (((gid < hid) << 1) | ((hid < mid) << 2) | ((mid < cid) << 3));

          G_P_M_C.push_back( __M(gid, hid, mid, cid) );
          G_P_M_C_Rel.push_back( __M(gid, hid, mid, cid, Rel) );
          G_P_M_C_Dir.push_back( __M(gid, hid, mid, cid, Dir) );

          Rel = (deprels[hid]<< 8| deprels[cid]);
          G_P_C.push_back( __M(gid, hid, cid) );
          G_P_C_Rel.push_back( __M(gid, hid, cid, Rel) );
          G_P_C_Dir.push_back( __M(gid, hid, cid, Dir) );

          Rel = (deprels[mid]<<8| deprels[cid]);
          G_M_C.push_back( __M(gid, mid, cid) );
          G_M_C_Rel.push_back( __M(gid, mid, cid, Rel) );
          G_M_C_Dir.push_back( __M(gid, mid, cid, Dir) );

          G_C.push_back( __M(gid, cid) );
          G_C_Rel.push_back( __M(gid, cid, deprels[cid]) );
          G_C_Dir.push_back( __M(gid, cid, Dir) );
        }
      }

      for (int k = 1; k < hnode.size(); ++ k) {
        int mid = hnode[k- 1], sid = hnode[k];
        if ((sid - hid) * (mid- hid) < 0) { continue; }

        int Rel = ((deprels[hid]<< 8 | deprels[mid]) << 8| deprels[sid]);
        int Dir = (((gid < hid) << 1) | ((hid < mid) << 2));

        G_P_M_S.push_back( __M(gid, hid, mid, sid) );
        G_P_M_S_Rel.push_back( __M(gid, hid, mid, sid, Rel) );
        G_P_M_S_Dir.push_back( __M(gid, hid, mid, sid, Dir) );

        Rel = deprels[mid]<<8 | deprels[sid];
        G_M_S.push_back( __M(gid, mid, sid) );
        G_M_S_Rel.push_back( __M(gid, mid, sid, Rel) );
        G_M_S_Dir.push_back( __M(gid, mid, sid, Dir) );
      }
    }
  }
}

int ScoreContext::non_punctuation_span_length(int now,
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

} //  namespace cstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
