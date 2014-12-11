#include "app/depparser/arceager/weight.h"
#include "app/depparser/arceager/action.h"
#include "app/depparser/arceager/score_context.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

#define _u(name) [](const ScoreContext& ctx, \
    std::vector<ufp_t>& cache) -> void{ \
  if (ctx.name) { \
    cache.push_back( ufp_t(ctx.name) ); \
  } \
}

#define _b(name1, name2) [](const ScoreContext& ctx, \
    std::vector<bfp_t>& cache) -> void{ \
  if (ctx.name1 && ctx.name2) { \
    cache.push_back( bfp_t( ctx.name1, ctx.name2) ); \
  } \
}

#define _t(name1, name2, name3) [](const ScoreContext& ctx, \
    std::vector<tfp_t>& cache) -> void{ \
  if (ctx.name1 && ctx.name2 && ctx.name3) { \
    cache.push_back( tfp_t(ctx.name1, ctx.name2, ctx.name3) ); \
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
  regist_unigram_feature( N0w );
  regist_unigram_feature( N0p );
  regist_bigram_feature( N0w, N0p );
  regist_unigram_feature( N1w );
  regist_unigram_feature( N1p );
  regist_bigram_feature( N1w, N1p );
  regist_unigram_feature( N2w );
  regist_unigram_feature( N2p );
  // There should be a quadgram feature in Zhang and Nirve (2011)
  // regist_quadgram_feature( S0w, S0p, N0w, N0p );
  regist_trigram_feature( S0w, S0p, N0w );
  regist_trigram_feature( S0w, N0w, N0p );
  regist_trigram_feature( S0w, S0p, N0p );
  regist_trigram_feature( S0p, N0w, N0p );
  regist_bigram_feature( S0w, N0w );
  regist_bigram_feature( S0p, N0p );
  regist_bigram_feature( N0p, N1p );
  regist_trigram_feature( N0p, N1p, N2p );
  regist_trigram_feature( S0p, N0p, N1p );
  regist_trigram_feature( S0hp, S0p, N0p );
  regist_trigram_feature( S0p, S0ldp, N0p );
  regist_trigram_feature( S0p, S0rdp, N0p );
  regist_trigram_feature( S0p, N0p, N0ldp );
  regist_bigram_feature( S0w, DistS0N0 );
  regist_bigram_feature( S0p, DistS0N0 );
  regist_bigram_feature( N0w, DistS0N0 );
  regist_bigram_feature( N0p, DistS0N0 );
  regist_trigram_feature( S0w, N0w, DistS0N0 );
  regist_trigram_feature( S0p, N0p, DistS0N0 );
  regist_bigram_feature( S0w, S0ra );
  regist_bigram_feature( S0p, S0ra );
  regist_bigram_feature( S0w, S0la );
  regist_bigram_feature( S0p, S0la );
  regist_bigram_feature( N0w, N0la );
  regist_bigram_feature( N0p, N0la );
  regist_unigram_feature( S0hw );
  regist_unigram_feature( S0hp );
  regist_unigram_feature( S0hl );
  regist_unigram_feature( S0ldw );
  regist_unigram_feature( S0ldp );
  regist_unigram_feature( S0ldl );
  regist_unigram_feature( S0rdw );
  regist_unigram_feature( S0rdp );
  regist_unigram_feature( S0rdl );
  regist_unigram_feature( N0ldw );
  regist_unigram_feature( N0ldp );
  regist_unigram_feature( N0ldl );
  regist_unigram_feature( S0h2w );
  regist_unigram_feature( S0h2p );
  regist_unigram_feature( S0h2l );
  regist_unigram_feature( S0l2dw );
  regist_unigram_feature( S0l2dp );
  regist_unigram_feature( S0l2dl );
  regist_unigram_feature( S0r2dw );
  regist_unigram_feature( S0r2dp );
  regist_unigram_feature( S0r2dl );
  regist_unigram_feature( N0l2dw );
  regist_unigram_feature( N0l2dp );
  regist_unigram_feature( N0l2dl );
  regist_trigram_feature( S0p, S0ldp, S0l2dp );
  regist_trigram_feature( S0p, S0rdp, S0r2dp );
  regist_trigram_feature( S0p, S0hp, S0h2p );
  regist_trigram_feature( N0p, N0ldp, N0l2dp );
  // There should also be the label set feature.
}

#undef _u
#undef _b
#undef _t

#undef regist_unigram_feature
#undef regist_bigram_feature
#undef regist_trigram_feature

} //  end for namespace arceager
} //  end for namespace dependencyparser
} //  end for namespace zuopar
