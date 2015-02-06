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
  typedef boost::tuple<int, int>                      T2;
  typedef boost::tuple<int, int, int>                 T3;
  typedef boost::tuple<int, int, int, int>            T4;
  typedef boost::tuple<int, int, int, int, int>       T5;
  typedef boost::tuple<int, int, int, int, int, int>  T6;
public:
  //! The score context class for extracting the global information.
  CostScoreContext(const State& state);
  int len;
  const std::vector<form_t>& forms;
  const std::vector<postag_t>& postags;
  const deprel_t* deprels;

  //! Begin singular
  /* 1 */ std::vector< int > H;
  /* 2 */ std::vector< T2 > H_H;
  /* 3 */ std::vector< T3 > H_H_H;
  /* 4 */ std::vector< T3 > H_pH_H;
  /* 5 */ std::vector< T3 > H_nH_H;
  /* 6 */ std::vector< T4 > pH_H_nH_H;
  //! End singular

  //! Begin first-order
  // - relation
  /* 7 */ std::vector< T2 > H_M;
  /* 8 */ std::vector< T3 > H_H_M;
  /* 9 */ std::vector< T3 > H_M_M;
  /* 10*/ std::vector< T4 > H_M_M_M;
  /* 11*/ std::vector< T4 > H_H_M_M;
  /* 12*/ std::vector< T5 > H_H_M_M_M;
  /* 13*/ std::vector< T3 > H_M_Dir;
  /* 14*/ std::vector< T3 > H_M_Dist;
  /* 15*/ std::vector< T4 > H_H_M_Dir;
  /* 16*/ std::vector< T4 > H_H_M_Dist;
  /* 17*/ std::vector< T4 > H_M_M_Dir;
  /* 18*/ std::vector< T4 > H_M_M_Dist;
  /* 19*/ std::vector< T5 > H_H_M_M_Dir;
  /* 20*/ std::vector< T5 > H_H_M_M_Dist;
  // - context
  /* 21*/ std::vector< T4 > pH_H_M_Mn;
  /* 22*/ std::vector< T3 > pH_H_M;
  /* 23*/ std::vector< T3 > H_M_Mn;
  /* 24*/ std::vector< T3 > pH_H_Mn;
  /* 25*/ std::vector< T3 > pH_M_Mn;
  /* 26*/ std::vector< T4 > H_nH_pM_M;
  /* 27*/ std::vector< T3 > H_nH_pM;
  /* 28*/ std::vector< T3 > nH_pM_M;
  /* 29*/ std::vector< T3 > H_pM_M;
  /* 30*/ std::vector< T3 > H_nH_M;
  //! End first-order

  //! Begin 2nd-order
  //! Begin 2nd-order sibling.
  // - relation
  std::vector< T3 > H_S_M;
  std::vector< T4 > H_S_M_Dir; // ! Dir is collasped.
  std::vector< T5 > H_S_M_S_M;
  std::vector< T2 > S_M;
  std::vector< T3 > S_M_Dir;
  std::vector< T4 > S_M_S_M;
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
  std::vector< T5 > G_P_M_P_M;
  std::vector< T4 > G_P_M_Dir;
  std::vector< T2 > G_M;
  std::vector< T3 > G_M_Dir;
  // - context
  std::vector< T4 > pG_G_P_M;
  std::vector< T4 > G_nG_P_M;
  std::vector< T4 > G_pP_P_M;
  std::vector< T4 > G_P_nP_M;
  std::vector< T4 > G_P_pM_M;
  std::vector< T4 > G_P_M_nM;
  //! End for 2nd-order sibling.

  //! Begin 3rd-order grandgrandparents
  std::vector< T4 > G_P_M_C;
  std::vector< T5 > G_P_M_C_Dir;
  std::vector< T3 > G_P_C;
  std::vector< T4 > G_P_C_Dir;
  std::vector< T3 > G_M_C;
  std::vector< T4 > G_M_C_Dir;
  std::vector< T2 > G_C;
  std::vector< T3 > G_C_Dir;
  //! End for 3rd-order grandgrandparents

  //! Begin 3rd-order grand-sibling
  std::vector< T4 > G_P_M_S;
  std::vector< T5 > G_P_M_S_Dir;
  std::vector< T3 > G_M_S;
  std::vector< T4 > G_M_S_Dir;
  //! End for 3rd-order grand-sibling

  //! Begin 3rd-order tri-sibling
  std::vector< T4 > H_M_S_T;
  std::vector< T5 > H_M_S_T_Dir;
  std::vector< T3 > H_M_T;
  std::vector< T4 > H_M_T_Dir;
  //! End for 3rd-order tri-sibling

  int RB;

  std::vector< boost::tuple<int, int, int, int> > head_bigrams;
  std::vector< boost::tuple<int, int, int, int> > outer_sibling_grandchildren;
  std::vector< boost::tuple<int, int, int, int> > inner_sibling_grandchildren;
};

} //  namespace arceager
} //  namespace dependencyparser
} //  namespace zuopar
#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_SCORE_CONTEXT_H__
