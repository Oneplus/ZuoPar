#include "app/depparser/arcstandard/weight.h"
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/score_context.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

#define _u(name) [](const ScoreContext& ctx, const Action& act, \
    std::vector<uf_t>& cache) -> void{ \
  if (ctx.name) { \
    cache.push_back( uf_t(ctx.name, act) ); \
  } \
}

#define _b(name1, name2) [](const ScoreContext& ctx, const Action& act, \
    std::vector<bf_t>& cache) -> void{ \
  if (ctx.name1 && ctx.name2) { \
    cache.push_back( bf_t(ctx.name1, ctx.name2, act) ); \
  } \
}

#define _t(name1, name2, name3) [](const ScoreContext& ctx, const Action& act, \
    std::vector<ts_t>& cache) -> void{ \
  if (ctx.name1 && ctx.name2 && ctx.name3) { \
    cache.push_back( tf_t(ctx.name1, ctx.name2, ctx.name3, act) ); \
  } \
}

#define regist_unigram_feature(name) do { \
  ufeat_map_repo.push_back( uf_map_t( _u(name) ) ); \
} while (0);

#define regist_bigram_feature(name1, name2) do { \
  bfeat_map_repo.push_back( bf_map_t( _b(name1, name2) ) ); \
} while (0);

#define regist_trigram_feature(name1, name2, name3) do { \
  tfeat_map_repo.push_back( tf_map_t( _t(name1, name2, name3) ) ); \
} while (0);

Weight::Weight() {
  regist_unigram_feature( S0w );
  regist_unigram_feature( S0p );
  regist_bigram_feature( S0w, S0p );
}

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
