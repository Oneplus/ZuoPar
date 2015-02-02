#include "experimental/hc_search/arceager/weight.h"
#include "experimental/hc_search/arceager/action.h"
#include "experimental/hc_search/arceager/score_context.h"

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
  // There should be a quadgram feature in Zhang and Nirve (2011)
  // regist_quadgram_feature( S0w, S0p, N0w, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_Q1111(S0w, S0p, N0w, N0p);
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0w, S0p, N0w );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0w, N0w, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0w, S0p, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, N0w, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S0w, N0w );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S0p, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( N0p, N1p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( N0p, N1p, N2p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, N0p, N1p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0hp, S0p, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S0ldp, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S0rdp, N0p );
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

CostWeight::CostWeight() {
  // arcs: forms
  bfeat_map_repo.push_back(bf_map_t(
        [](const CostScoreContext& ctx, std::vector<bf_t>& cache) -> void {
          for (const std::pair<int, int>& two: ctx.arcs) {
            cache.push_back(bf_t(
                ctx.forms[two.first],
                ctx.forms[two.second]));
          }
        }));

  // arcs: postags
  bfeat_map_repo.push_back(bf_map_t(
        [](const CostScoreContext& ctx, std::vector<bf_t>& cache) -> void {
          for (const std::pair<int, int>& two: ctx.arcs) {
            cache.push_back(bf_t(
                ctx.postags[two.first],
                ctx.postags[two.second]));
          }
        }));

  // arcs: deprels(1)
  bfeat_map_repo.push_back(bf_map_t(
        [](const CostScoreContext& ctx, std::vector<bf_t>& cache) -> void {
          for (const std::pair<int, int>& two: ctx.arcs) {
            cache.push_back(bf_t(
                ctx.forms[two.first],
                ctx.deprels[two.second]));
          }
        }));

  // arcs: deprels(2)
  bfeat_map_repo.push_back(bf_map_t(
        [](const CostScoreContext& ctx, std::vector<bf_t>& cache) -> void {
          for (const std::pair<int, int>& two: ctx.arcs) {
            cache.push_back(bf_t(
                ctx.postags[two.first],
                ctx.deprels[two.second]));
          }
        }));

  // consecutive sibling: forms
  tfeat_map_repo.push_back(tf_map_t(
        [](const CostScoreContext& ctx, std::vector<tf_t>& cache) -> void {
          for (const boost::tuple<int, int, int>& three: ctx.consecutive_siblings) {
            cache.push_back(tf_t(
                ctx.forms[three.get<0>()],
                ctx.forms[three.get<1>()],
                ctx.forms[three.get<2>()]));
          }
        }));

  // consecutive sibling: postags
  tfeat_map_repo.push_back(tf_map_t(
        [](const CostScoreContext& ctx, std::vector<tf_t>& cache) -> void {
          for (const boost::tuple<int, int, int>& three: ctx.consecutive_siblings) {
            cache.push_back(tf_t(
                ctx.postags[three.get<0>()],
                ctx.postags[three.get<1>()],
                ctx.postags[three.get<2>()]));
          }
        }));

  // consecutive sibling: deprels(1)
  tfeat_map_repo.push_back(tf_map_t(
        [](const CostScoreContext& ctx, std::vector<tf_t>& cache) -> void {
          for (const boost::tuple<int, int, int>& three: ctx.consecutive_siblings) {
            cache.push_back(tf_t(
                ctx.forms[three.get<0>()],
                ctx.deprels[three.get<1>()],
                ctx.deprels[three.get<2>()]));
          }
        }));

  // consecutive sibling: deprels(2)
  tfeat_map_repo.push_back(tf_map_t(
        [](const CostScoreContext& ctx, std::vector<tf_t>& cache) -> void {
          for (const boost::tuple<int, int, int>& three: ctx.consecutive_siblings) {
            cache.push_back(tf_t(
                ctx.postags[three.get<0>()],
                ctx.deprels[three.get<1>()],
                ctx.deprels[three.get<2>()]));
          }
        }));

  // grand parents: forms
  tfeat_map_repo.push_back(tf_map_t(
        [](const CostScoreContext& ctx, std::vector<tf_t>& cache) -> void {
          for (const boost::tuple<int, int, int>& three: ctx.grandparents) {
            cache.push_back(tf_t(
                ctx.forms[three.get<0>()],
                ctx.forms[three.get<1>()],
                ctx.forms[three.get<2>()]));
          }
        }));

  // grand parents: postags
  tfeat_map_repo.push_back(tf_map_t(
        [](const CostScoreContext& ctx, std::vector<tf_t>& cache) -> void {
          for (const boost::tuple<int, int, int>& three: ctx.grandparents) {
            cache.push_back(tf_t(
                ctx.postags[three.get<0>()],
                ctx.postags[three.get<1>()],
                ctx.postags[three.get<2>()]));
          }
        }));

  // head bigram: forms
  qfeat_map_repo.push_back(qf_map_t(
        [](const CostScoreContext& ctx, std::vector<qf_t>& cache) -> void {
          for (const boost::tuple<int, int, int, int>& four: ctx.head_bigrams) {
            cache.push_back(qf_t(
                ctx.forms[four.get<0>()],
                ctx.forms[four.get<1>()],
                ctx.forms[four.get<2>()],
                ctx.forms[four.get<3>()]));
          }
        }));

  // head bigram: postags
  qfeat_map_repo.push_back(qf_map_t(
        [](const CostScoreContext& ctx, std::vector<qf_t>& cache) -> void {
          for (const boost::tuple<int, int, int, int>& four: ctx.head_bigrams) {
            cache.push_back(qf_t(
                ctx.postags[four.get<0>()],
                ctx.postags[four.get<1>()],
                ctx.postags[four.get<2>()],
                ctx.postags[four.get<3>()]));
          }
        }));
/*
  qfeat_map_repo.push_back(qf_map_t(
        [](const CostScoreContext& ctx, std::vector<qf_t>& cache) -> void {
          for (const boost::tuple<int, int, int, int>& four: ctx.grand_siblings) {
            cache.push_back(qf_t(four.get<0>(), four.get<1>(), four.get<2>(), four.get<3>()));
          }
        }));
  qfeat_map_repo.push_back(qf_map_t(
        [](const CostScoreContext& ctx, std::vector<qf_t>& cache) -> void {
          for (const boost::tuple<int, int, int, int>& four: ctx.tri_siblings) {
            cache.push_back(qf_t(four.get<0>(), four.get<1>(), four.get<2>(), four.get<3>()));
          }
        }));
  qfeat_map_repo.push_back(qf_map_t(
        [](const CostScoreContext& ctx, std::vector<qf_t>& cache) -> void {
          for (const boost::tuple<int, int, int, int>& four: ctx.grand_grandparents) {
            cache.push_back(qf_t(four.get<0>(), four.get<1>(), four.get<2>(), four.get<3>()));
          }
        }));
  qfeat_map_repo.push_back(qf_map_t(
        [](const CostScoreContext& ctx, std::vector<qf_t>& cache) -> void {
          for (const boost::tuple<int, int, int, int>& four: ctx.outer_sibling_grandchildren) {
            cache.push_back(qf_t(four.get<0>(), four.get<1>(), four.get<2>(), four.get<3>()));
          }
        }));
  qfeat_map_repo.push_back(qf_map_t(
        [](const CostScoreContext& ctx, std::vector<qf_t>& cache) -> void {
          for (const boost::tuple<int, int, int, int>& four: ctx.inner_sibling_grandchildren) {
            cache.push_back(qf_t(four.get<0>(), four.get<1>(), four.get<2>(), four.get<3>()));
          }
        }));
*/
}

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
