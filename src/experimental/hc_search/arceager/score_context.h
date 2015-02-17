#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_SCORE_CONTEXT_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_SCORE_CONTEXT_H__

#include <vector>
#include <boost/tuple/tuple.hpp>
#include "types/common.h"
#include "utils/math/fast_binned.h"
#include "experimental/hc_search/arceager/state.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

class HeuristicScoreContext {
public:
  /**
   * The ScoreContext constructor
   *
   *  @param[in]  state   The State
   */
  HeuristicScoreContext(const State& state);

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

class CostScoreContext {
public:
  typedef boost::tuple<int, int>                          T2;
  typedef boost::tuple<int, int, int>                     T3;
  typedef boost::tuple<int, int, int, int>                T4;
  typedef boost::tuple<int, int, int, int, int>           T5;
  typedef boost::tuple<int, int, int, int, int, int>      T6;
  typedef boost::tuple<int, int, int, int, int, int, int> T7;
public:
  //! The score context class for extracting the global information.
  CostScoreContext(const State& state);
  int len;
  const std::vector<form_t>& forms;
  const std::vector<postag_t>& postags;
  const deprel_t* deprels;
  //! Begin singular
  std::vector< int > H;
  std::vector< T2 > H_H;
  std::vector< T3 > H_H_H;
  std::vector< T3 > H_pH_H;
  std::vector< T3 > H_nH_H;
  std::vector< T4 > pH_H_nH_H;
  //! End singular

  //! Begin first-order
  // - relation
  std::vector< T2 > H_M;
  std::vector< T3 > H_H_M;
  std::vector< T3 > H_M_M;
  std::vector< T4 > H_H_M_M;
  std::vector< T3 > H_M_Rel;
  std::vector< T4 > H_M_M_Rel;
  std::vector< T4 > H_H_M_Rel;
  std::vector< T5 > H_H_M_M_Rel;
  std::vector< T3 > H_M_Dir;
  std::vector< T3 > H_M_Dist;
  std::vector< T4 > H_H_M_Dir;
  std::vector< T4 > H_H_M_Dist;
  std::vector< T4 > H_M_M_Dir;
  std::vector< T4 > H_M_M_Dist;
  std::vector< T5 > H_H_M_M_Dir;
  std::vector< T5 > H_H_M_M_Dist;
  // - context
  std::vector< T4 > pH_H_M_Mn;
  std::vector< T3 > pH_H_M;
  std::vector< T3 > H_M_Mn;
  std::vector< T3 > pH_H_Mn;
  std::vector< T3 > pH_M_Mn;
  std::vector< T4 > H_nH_pM_M;
  std::vector< T3 > H_nH_pM;
  std::vector< T3 > nH_pM_M;
  std::vector< T3 > H_pM_M;
  std::vector< T3 > H_nH_M;
  //! End first-order

  //! Begin 2nd-order
  //! Begin 2nd-order sibling.
  // - relation
  std::vector< T3 > H_S_M;
  std::vector< T4 > H_S_M_Dir; // ! Dir is collasped.
  std::vector< T4 > H_S_M_Rel;
  std::vector< T2 > S_M;
  std::vector< T3 > S_M_Dir;
  std::vector< T3 > S_M_Rel;
  // - context
  std::vector< T4 > pH_H_S_M;
  std::vector< T4 > H_nH_S_M;
  std::vector< T4 > H_pS_S_M;
  std::vector< T4 > H_S_nS_M;
  std::vector< T4 > H_S_pM_M;
  std::vector< T4 > H_S_M_nM;
  //! End for 2nd-order sibling.

  //! Begin 2nd-order grandparent.
  // - relation
  std::vector< T3 > G_P_M;
  std::vector< T4 > G_P_M_Rel;
  std::vector< T4 > G_P_M_Dir;
  std::vector< T2 > G_M;
  std::vector< T3 > G_M_Rel;
  std::vector< T3 > G_M_Dir;
  // - context
  std::vector< T4 > pG_G_P_M;
  std::vector< T4 > G_nG_P_M;
  std::vector< T4 > G_pP_P_M;
  std::vector< T4 > G_P_nP_M;
  std::vector< T4 > G_P_pM_M;
  std::vector< T4 > G_P_M_nM;
  //! End for 2nd-order sibling.

  //! Begin 2nd-order head bigram.
  // - relation
  std::vector< T4 > H1_H_M1_M;
  std::vector< T5 > H1_H_M1_M_Dir;
  //! End for 2nd-order head bigram.

  //! Begin 3rd-order grandgrandparents
  std::vector< T4 > G_P_M_C;
  std::vector< T5 > G_P_M_C_Rel;
  std::vector< T5 > G_P_M_C_Dir;
  std::vector< T3 > G_P_C;
  std::vector< T4 > G_P_C_Rel;
  std::vector< T4 > G_P_C_Dir;
  std::vector< T3 > G_M_C;
  std::vector< T4 > G_M_C_Rel;
  std::vector< T4 > G_M_C_Dir;
  std::vector< T2 > G_C;
  std::vector< T3 > G_C_Rel;
  std::vector< T3 > G_C_Dir;
  //! End for 3rd-order grandgrandparents

  //! Begin 3rd-order grand-sibling
  std::vector< T4 > G_P_M_S;
  std::vector< T5 > G_P_M_S_Rel;
  std::vector< T5 > G_P_M_S_Dir;
  std::vector< T3 > G_M_S;
  std::vector< T4 > G_M_S_Rel;
  std::vector< T4 > G_M_S_Dir;
  //! End for 3rd-order grand-sibling

  //! Begin 3rd-order tri-sibling
  std::vector< T4 > H_M_S_T;
  std::vector< T5 > H_M_S_T_Rel;
  std::vector< T5 > H_M_S_T_Dir;
  std::vector< T3 > H_M_T;
  std::vector< T4 > H_M_T_Rel;
  std::vector< T4 > H_M_T_Dir;
  //! End for 3rd-order tri-sibling

  //! Begin 3rd-order pp attachment.
  std::vector< T2 > PP_H_M;
  std::vector< T3 > PP_P_H_M;
  std::vector< T3 > PP_H_M_Dir;
  std::vector< T4 > PP_P_H_M_Dir;
  //! End for 3rd-order pp attachement.

  //! Begin 3rd-order inner-sibling-grandchildren
  std::vector< T4 > H_S_M_GC;
  std::vector< T5 > H_S_M_GC_Rel;
  std::vector< T5 > H_S_M_GC_Dir;
  //! End for 3rd-order inner-sibling-grandchildren

  //! Begin 3rd-order outer-sibling-grandchildren
  std::vector< T4 > H_S_GC_M;
  std::vector< T5 > H_S_GC_M_Rel;
  std::vector< T5 > H_S_GC_M_Dir;
  //! End for 3rd-order outer-sibling-grandchildren

  //! Begin 3rd-order span length
  std::vector< int > span_length;
  //! End for 3rd-order span length;

  //! Begin 3rd-order valency
  std::vector< int > nr_children;
  std::vector< int > nr_left_children;
  std::vector< int > nr_right_children;
  //! End for 3rd-order valency

  //! Begin 3rd-order label set.
  std::vector< int > label_set;
  std::vector< int > left_label_set;
  std::vector< int > right_label_set;
  //! End for 3rd-order label set.

  //! Right branch
  int RB;

  int RANK;

  int SCORE;
  //! Coordination

private:
  int non_punctuation_span_length(int now,
      const std::vector<postag_t>& postags,
      const std::vector<std::vector<int> >& tree,
      std::vector<int>& result);
};

} //  namespace arceager
} //  namespace dependencyparser
} //  namespace zuopar
#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_SCORE_CONTEXT_H__
