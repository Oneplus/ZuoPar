#include "app/depparser/arcstandard/weight.h"
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/score_context.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

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
  regist_unigram_feature( S1w );
  regist_unigram_feature( S1p );
  regist_bigram_feature( S1w, S1p );
  regist_unigram_feature( N0w );
  regist_unigram_feature( N0p );
  regist_bigram_feature( N0w, N0p );
  regist_unigram_feature( N1w );
  regist_unigram_feature( N1p );
  regist_bigram_feature( N1w, N1p );  // port "from single words" in Z&N (2011)
  regist_trigram_feature( S0w, S0p, S1w );
  regist_trigram_feature( S0w, S1w, S1p );
  regist_trigram_feature( S0w, S0p, N0p );
  regist_trigram_feature( S0p, S1w, S1p );
  regist_bigram_feature( S0w, S1w );
  regist_bigram_feature( S0p, S1p );
  regist_bigram_feature( S0p, N0p );  //  port "from word pairs" in Z&N (2011)
  regist_trigram_feature( S0p, N0p, N1p );
  regist_trigram_feature( S1p, S0p, N0p );
  regist_trigram_feature( S0p, S0ldp, S1p );
  regist_trigram_feature( S0p, S0rdp, S1p );
  regist_trigram_feature( S0p, S1ldp, S1p );
  regist_trigram_feature( S0p, S1rdp, S1p );  //  port the "from three words" in Z&N (2011)
  regist_bigram_feature( S0w, DistS0S1 );
  regist_bigram_feature( S0p, DistS0S1 );
  regist_bigram_feature( S1w, DistS0S1 );
  regist_bigram_feature( S1p, DistS0S1 );
  regist_trigram_feature( S0w, S1w, DistS0S1 );
  regist_trigram_feature( S0p, S1p, DistS0S1 ); //  port the "distance" in Z&N (2011)
  regist_bigram_feature( S0w, S0la );
  regist_bigram_feature( S0w, S0ra );
  regist_bigram_feature( S0p, S0la );
  regist_bigram_feature( S0p, S0ra );
  regist_bigram_feature( S1w, S1la );
  regist_bigram_feature( S1w, S1ra );
  regist_bigram_feature( S1p, S1la );
  regist_bigram_feature( S1p, S1ra ); //  port the "valency" in Z&N (2011)
  regist_unigram_feature( S0ldw );
  regist_unigram_feature( S0ldp );
  regist_unigram_feature( S0ldl );
  regist_unigram_feature( S0rdw );
  regist_unigram_feature( S0rdp );
  regist_unigram_feature( S0rdl );
  regist_unigram_feature( S1ldw );
  regist_unigram_feature( S1ldp );
  regist_unigram_feature( S1ldl );
  regist_unigram_feature( S1rdw );
  regist_unigram_feature( S1rdp );
  regist_unigram_feature( S1rdl );  // port the "unigram" in Z&N (2011)
  regist_unigram_feature( S0l2dw );
  regist_unigram_feature( S0l2dp );
  regist_unigram_feature( S0l2dl );
  regist_unigram_feature( S0r2dw );
  regist_unigram_feature( S0r2dp );
  regist_unigram_feature( S0r2dl );
  regist_trigram_feature( S0p, S0ldp, S0l2dp );
  regist_trigram_feature( S0p, S0rdp, S0r2dp );
  regist_trigram_feature( S1p, S1ldp, S1l2dp );
  regist_trigram_feature( S1p, S1rdp, S1r2dp ); //  port the "third-order" in Z&N (2011)
  //regist_trigram_feature( S0p, S1p, S0r2dp );
  //regist_trigram_feature( S0p, S1p, S0l2dp );
  //regist_trigram_feature( S0p, S1p, S1r2dp );
  //regist_trigram_feature( S0p, S1p, S1l2dp );
}

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
