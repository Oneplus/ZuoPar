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
  //! The score context class for extracting the global information.
  CostScoreContext(const State& state);
  int len;
  const std::vector<form_t>& forms;
  const std::vector<postag_t>& postags;
  const deprel_t* deprels;
  std::vector< int > H;
  std::vector< std::pair<int, int> > H_H;
  std::vector< boost::tuple<int, int, int> > H_H_H;
  std::vector< boost::tuple<int, int, int> > H_H_pH;
  std::vector< boost::tuple<int, int, int> > H_H_nH;
  std::vector< boost::tuple<int, int, int, int > > H_pH_H_nH;
  std::vector< std::pair<int, int> > H_M;
  std::vector< boost::tuple<int, int, int, int > > pH_H_M_Mn;
  std::vector< boost::tuple<int, int, int > > pH_H_M;
  std::vector< boost::tuple<int, int, int > > H_M_Mn;
  std::vector< boost::tuple<int, int, int > > pH_H_Mn;
  std::vector< boost::tuple<int, int, int > > pH_M_Mn;
  std::vector< boost::tuple<int, int, int, int > > H_nH_pM_M;
  std::vector< boost::tuple<int, int, int > > H_nH_pM;
  std::vector< boost::tuple<int, int, int > > nH_pM_M;
  std::vector< boost::tuple<int, int, int > > H_pM_M;
  std::vector< boost::tuple<int, int, int > > H_nH_M;
  std::vector< boost::tuple<int, int, int > > H_H_M;
  std::vector< boost::tuple<int, int, int > > H_M_M;
  std::vector< boost::tuple<int, int, int, int > > H_H_M_M;
  std::vector< boost::tuple<int, int, int> > consecutive_siblings;
  std::vector< boost::tuple<int, int, int> > grandparents;
  std::vector< boost::tuple<int, int, int, int> > head_bigrams;
  std::vector< boost::tuple<int, int, int, int> > grand_siblings;
  std::vector< boost::tuple<int, int, int, int> > tri_siblings;
  std::vector< boost::tuple<int, int, int, int> > grand_grandparents;
  std::vector< boost::tuple<int, int, int, int> > outer_sibling_grandchildren;
  std::vector< boost::tuple<int, int, int, int> > inner_sibling_grandchildren;
};

} //  namespace arceager
} //  namespace dependencyparser
} //  namespace zuopar
#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_SCORE_CONTEXT_H__
