#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_MODEL_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_MODEL_H__

#include "types/common.h"
#include "model/fast_meta/meta_feature_param_map_collection.h"
#include "model/fast_associated/feature_param_map_collection.h"
#include "experimental/hc_search/arceager/action.h"
#include "experimental/hc_search/arceager/state.h"
#include "experimental/hc_search/arceager/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

class HeuristicWeight
: public FeatureParameterCollection<State, HeuristicScoreContext, Action> {
public:
  //! The weight constructor.
  HeuristicWeight();
};

class CostWeight
: public MetaFeatureParameterCollection<State, CostScoreContext> {
public:
  //! The weight constructor.
  CostWeight();
};

#define G(id, _W) (((id) < 0) ? 1: (((id) >= ctx.len) ? 2: ctx._W[(id)]))

#define __REGN_1(name) do {                                                 \
  ufeat_map_repo.push_back(uf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<uf_t>& cache) -> void { \
          for (const int& one: ctx.name) { cache.push_back(one); }          \
        }));                                                                \
} while (0);

#define __REG_1(name, _W1) do {                                             \
  ufeat_map_repo.push_back(uf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<uf_t>& cache) -> void { \
          for (const int& one: ctx.name) {                                  \
            cache.push_back(uf_t(G(one, _W1)));                             \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REG_2(name, _W1, _W2) do {                                        \
  bfeat_map_repo.push_back(bf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<bf_t>& cache) -> void { \
          for (const CostScoreContext::T2& two: ctx.name) {                 \
            cache.push_back(bf_t(                                           \
                G(two.get<0>(), _W1),                                       \
                G(two.get<1>(), _W2))) ;                                    \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REGN_2(name, _W1) do {                                            \
  bfeat_map_repo.push_back(bf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<bf_t>& cache) -> void { \
          for (const CostScoreContext::T2& two: ctx.name) {                 \
            cache.push_back(bf_t(                                           \
                G(two.get<0>(), _W1),                                       \
                two.get<1>()));                                             \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REG_3(name, _W1, _W2, _W3) do {                                   \
  tfeat_map_repo.push_back(tf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<tf_t>& cache) -> void { \
          for (const CostScoreContext::T3& three: ctx.name) {               \
            cache.push_back(tf_t(                                           \
                G(three.get<0>(), _W1),                                     \
                G(three.get<1>(), _W2),                                     \
                G(three.get<2>(), _W3)));                                   \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REGN_3(name, _W1, _W2) do {                                       \
  tfeat_map_repo.push_back(tf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<tf_t>& cache) -> void { \
          for (const CostScoreContext::T3& three: ctx.name) {               \
            cache.push_back(tf_t(                                           \
                G(three.get<0>(), _W1),                                     \
                G(three.get<1>(), _W2),                                     \
                three.get<2>()));                                           \
          }                                                                 \
        }));                                                                \
} while (0);


#define __REG_4(name, _W1, _W2, _W3, _W4) do {                              \
  qfeat_map_repo.push_back(qf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<qf_t>& cache) -> void { \
          for (const CostScoreContext::T4& four: ctx.name) {                \
            cache.push_back(qf_t(                                           \
                G(four.get<0>(), _W1),                                      \
                G(four.get<1>(), _W2),                                      \
                G(four.get<2>(), _W3),                                      \
                G(four.get<3>(), _W4)));                                    \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REGN_4(name, _W1, _W2, _W3) do {                                  \
  qfeat_map_repo.push_back(qf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<qf_t>& cache) -> void { \
          for (const CostScoreContext::T4& four: ctx.name) {                \
            cache.push_back(qf_t(                                           \
                G(four.get<0>(), _W1),                                      \
                G(four.get<1>(), _W2),                                      \
                G(four.get<2>(), _W3),                                      \
                four.get<3>()));                                            \
          }                                                                 \
        }));                                                                \
} while (0);


#define __REG_5(name, _W1, _W2, _W3, _W4, _W5) do {                         \
  q5feat_map_repo.push_back(q5f_map_t(                                      \
        [](const CostScoreContext& ctx, std::vector<q5f_t>& cache) -> void {\
          for (const CostScoreContext::T5& five: ctx.name) {                \
            cache.push_back(q5f_t(                                          \
                G(five.get<0>(), _W1),                                      \
                G(five.get<1>(), _W2),                                      \
                G(five.get<2>(), _W3),                                      \
                G(five.get<3>(), _W4),                                      \
                G(five.get<4>(), _W5)));                                    \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REGN_5(name, _W1, _W2, _W3, _W4) do {                             \
  q5feat_map_repo.push_back(q5f_map_t(                                      \
        [](const CostScoreContext& ctx, std::vector<q5f_t>& cache) -> void {\
          for (const CostScoreContext::T5& five: ctx.name) {                \
            cache.push_back(q5f_t(                                          \
                G(five.get<0>(), _W1),                                      \
                G(five.get<1>(), _W2),                                      \
                G(five.get<2>(), _W3),                                      \
                G(five.get<3>(), _W4),                                      \
                five.get<4>()));                                            \
          }                                                                 \
        }));                                                                \
} while (0);

#define __REG_6(name, _W1, _W2, _W3, _W4, _W5, _W6) do {                    \
  hfeat_map_repo.push_back(hf_map_t(                                        \
        [](const CostScoreContext& ctx, std::vector<hf_t>& cache) -> void { \
          for (const CostScoreContext::T6& six: ctx.name) {                 \
            cache.push_back(q5f_t(                                          \
                G(six.get<0>(), _W1),                                       \
                G(six.get<1>(), _W2),                                       \
                G(six.get<2>(), _W3),                                       \
                G(six.get<3>(), _W4),                                       \
                G(six.get<4>(), _W5),                                       \
                G(six.get<5>(), _W6));                                      \
          }                                                                 \
        }));                                                                \
} while (0);

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_MODEL_H__
