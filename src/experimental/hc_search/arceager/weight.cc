#include "experimental/hc_search/arceager/weight.h"
#include "experimental/hc_search/arceager/action.h"
#include "experimental/hc_search/arceager/score_context.h"
#include "utils/math/fast_binned.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

typedef HeuristicScoreContext ScoreContext;

HeuristicWeight::HeuristicWeight() {
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0w );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S0w, S0p );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N0w );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N0p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( N0w, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N1w );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N1p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( N1w, N1p );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N2w );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N2p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( N2w, N2p );
  // There should be a quadgram feature in Zhang and Nirve (2011)
  // regist_quadgram_feature( S0w, S0p, N0w, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_Q1100(S0w, S0p, N0w, N0p);
  ZUOPAR_FEATURE_MAP_REGIST_T110( S0w, S0p, N0w );
  ZUOPAR_FEATURE_MAP_REGIST_T100( S0w, N0w, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_T110( S0w, S0p, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_T100( S0p, N0w, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0w, N0w );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0p, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_B10( N0p, N1p );
  ZUOPAR_FEATURE_MAP_REGIST_T100( N0p, N1p, N2p );
  ZUOPAR_FEATURE_MAP_REGIST_T110( S0p, N0p, N1p );
  ZUOPAR_FEATURE_MAP_REGIST_T110( S0hp, S0p, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_T110( S0p, S0ldp, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_T110( S0p, S0rdp, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, N0p, N0ldp );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S0w, DistS0N0 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S0p, DistS0N0 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( N0w, DistS0N0 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( N0p, DistS0N0 );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0w, N0w, DistS0N0 );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, N0p, DistS0N0 );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0w, S0ra );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0p, S0ra );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0w, S0la );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0p, S0la );
  ZUOPAR_FEATURE_MAP_REGIST_B10( N0w, N0la );
  ZUOPAR_FEATURE_MAP_REGIST_B10( N0p, N0la );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0hw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0hp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0hl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0ldw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0ldp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0ldl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0rdw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0rdp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0rdl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N0ldw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N0ldp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N0ldl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0h2w );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0h2p );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0h2l );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0l2dw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0l2dp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0l2dl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0r2dw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0r2dp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0r2dl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N0l2dw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N0l2dp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N0l2dl );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S0ldp, S0l2dp );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S0rdp, S0r2dp );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S0hp, S0h2p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( N0p, N0ldp, N0l2dp );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0w, S0lset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0p, S0lset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0w, S0rset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0p, S0rset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( N0w, N0lset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( N0p, N0lset );
}

#define G(id, _W) (((id) < 0) ? 1: (((id) >= ctx.len) ? 2: ctx._W[(id)]))

#define __REG_U(name, _W1) do {                                             \
  ufeat_map_repo.push_back(uf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<uf_t>& cache) -> void { \
          for (const int& one: ctx.name) {                                  \
            cache.push_back(uf_t(G(one, _W1)));                             \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REG_B(name, _W1, _W2) do {                                        \
  bfeat_map_repo.push_back(bf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<bf_t>& cache) -> void { \
          for (const std::pair<int, int>& two: ctx.name) {                  \
            cache.push_back(bf_t(                                           \
                G(two.first, _W1),                                          \
                G(two.second, _W2))) ;                                      \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REG_B_DIR(name, _W1, _W2) do {                                    \
  tfeat_map_repo.push_back(tf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<tf_t>& cache) -> void { \
          for (const std::pair<int, int>& two: ctx.name) {                  \
            cache.push_back(tf_t(                                           \
                G(two.first, _W1),                                          \
                G(two.second, _W2),                                         \
                (two.first < two.second? 1: 2)));                           \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REG_B_DIST(name, _W1, _W2) do {                                   \
  tfeat_map_repo.push_back(tf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<tf_t>& cache) -> void { \
          for (const std::pair<int, int>& two: ctx.name) {                  \
            cache.push_back(tf_t(                                           \
                G(two.first, _W1),                                          \
                G(two.second, _W2),                                         \
                (two.first < two.second?                                    \
                 Math::binned_1_2_3_4_5_6_10[two.second- two.first]:        \
                 Math::binned_1_2_3_4_5_6_10[two.first- two.second])));     \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REG_T(name, _W1, _W2, _W3) do {                                   \
  tfeat_map_repo.push_back(tf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<tf_t>& cache) -> void { \
          for (const boost::tuple<int, int, int>& three: ctx.name) {        \
            cache.push_back(tf_t(                                           \
                G(three.get<0>(), _W1),                                     \
                G(three.get<1>(), _W2),                                     \
                G(three.get<2>(), _W3)));                                   \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REG_T_DIST(name, _W1, _W2, _W3) do {                                \
  qfeat_map_repo.push_back(qf_map_t(                                          \
        [](const CostScoreContext& ctx, std::vector<qf_t>& cache) -> void {   \
          for (const boost::tuple<int, int, int>& three: ctx.name) {          \
            cache.push_back(qf_t(                                             \
                G(three.get<0>(), _W1),                                       \
                G(three.get<1>(), _W2),                                       \
                G(three.get<2>(), _W3),                                       \
                (three.get<0>() < three.get<2>()?                             \
                 Math::binned_1_2_3_4_5_6_10[three.get<2>()- three.get<0>()]: \
                 Math::binned_1_2_3_4_5_6_10[three.get<0>()- three.get<2>()]) \
                ));                                                           \
          }                                                                   \
        }));                                                                  \
} while (0);


#define __REG_Q(name, _W1, _W2, _W3, _W4) do {                              \
  qfeat_map_repo.push_back(qf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<qf_t>& cache) -> void { \
          for (const boost::tuple<int, int, int, int>& four: ctx.name) {    \
            cache.push_back(qf_t(                                           \
                G(four.get<0>(), _W1),                                      \
                G(four.get<1>(), _W2),                                      \
                G(four.get<2>(), _W3),                                      \
                G(four.get<3>(), _W4)));                                    \
          }                                                                 \
        }));                                                                \
} while (0);

CostWeight::CostWeight() {
  //! First-Order features.
  //! Head
  __REG_U(H, deprels);
  __REG_B(H_H, deprels, postags);
  __REG_B(H_H, deprels, forms);
  __REG_T(H_H_H, deprels, forms, postags);
  __REG_T(H_H_pH, deprels, postags, postags);
  __REG_T(H_H_nH, deprels, postags, postags);
  __REG_Q(H_pH_H_nH, deprels, postags, postags, postags);
  //! Head-Modifier:
  __REG_B(H_M, forms, forms);
  __REG_B(H_M, forms, postags);
  __REG_B(H_M, postags, forms);
  __REG_B(H_M, postags, postags);
  __REG_T(H_M_M, postags, postags, forms);
  __REG_T(H_M_M, forms, postags, forms);
  __REG_T(H_H_M, forms, postags, postags);
  __REG_T(H_H_M, forms, postags, forms);
  __REG_Q(H_H_M_M, forms, postags, forms, postags);
  __REG_Q(pH_H_M_Mn, postags, postags, postags, postags);
  __REG_T(pH_H_M, postags, postags, postags);
  __REG_T(H_M_Mn, postags, postags, postags);
  __REG_T(pH_H_Mn, postags, postags, postags);
  __REG_T(pH_M_Mn, postags, postags, postags);
  __REG_Q(H_nH_pM_M, postags, postags, postags, postags);
  __REG_T(H_nH_M, postags, postags, postags);
  __REG_T(H_pM_M, postags, postags, postags);
  __REG_T(nH_pM_M, postags, postags, postags);
  __REG_T(H_nH_pM, postags, postags, postags);

  //! Head-Modifier: distance argumented.
  __REG_B_DIST(H_M, forms, forms);
  __REG_B_DIST(H_M, forms, postags);
  __REG_B_DIST(H_M, postags, forms);
  __REG_B_DIST(H_M, postags, postags);
  __REG_T_DIST(H_M_M, postags, postags, forms);
  __REG_T_DIST(H_M_M, forms, postags, forms);
  __REG_T_DIST(H_H_M, forms, postags, postags);
  __REG_T_DIST(H_H_M, forms, postags, forms);
  __REG_T(H_M_M, postags, postags, deprels);
  __REG_T(H_M_M, postags, forms, deprels);
  __REG_T(H_M_M, forms, postags, deprels);
  __REG_T(H_M_M, forms, forms, deprels);
  __REG_T_DIST(H_M_M, postags, postags, deprels);
  __REG_T_DIST(H_M_M, forms, postags, deprels);
  __REG_T_DIST(H_M_M, postags, forms, deprels);
  __REG_T_DIST(H_M_M, forms, forms, deprels);

  //! consecutive sibling: forms
  __REG_T(consecutive_siblings, forms, forms, forms);
  //! consecutive sibling: postags
  __REG_T(consecutive_siblings, postags, postags, postags);
  //! consecutive sibling: deprels(1)
  __REG_T(consecutive_siblings, forms, deprels, deprels);
  //! consecutive sibling: deprels(1)
  __REG_T(consecutive_siblings, postags, deprels, deprels);
  //! grand parents: forms
  //__REG_T(grandparents, forms, forms, forms);
  //! grand parents: postags
  __REG_T(grandparents, postags, postags, postags);
  //! head bigram: forms
  //__REG_Q(head_bigrams, forms, forms, forms, forms);
  //! head bigram: postags
  __REG_Q(head_bigrams, postags, postags, postags, postags);
  //! grand sibling: forms
  //__REG_Q(grand_siblings, forms, forms, forms, forms);
  //! grand sibling: postags
  __REG_Q(grand_siblings, postags, postags, postags, postags);
  //! tri sibling: forms
  //__REG_Q(tri_siblings, forms, forms, forms, forms);
  //! tri sibling: postags
  __REG_Q(tri_siblings, postags, postags, postags, postags);
  //! grad grand parent: forms
  //__REG_Q(grand_grandparents, forms, forms, forms, forms);
  //! grad grand parent: postags
  __REG_Q(grand_grandparents, postags, postags, postags, postags);
  //! outer sibling grandchild: forms
  //__REG_Q(outer_sibling_grandchildren, forms, forms, forms, forms);
  //! outer sibling grandchild: postags
  __REG_Q(outer_sibling_grandchildren, postags, postags, postags, postags);
  //! inter sibling grandchild: forms
  //__REG_Q(inner_sibling_grandchildren, forms, forms, forms, forms);
  //! inter sibling grandchild: postags
  __REG_Q(inner_sibling_grandchildren, postags, postags, postags, postags)
}

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
