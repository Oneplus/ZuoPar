#include "app/lexanalyzer/postagger/weight.h"
#include "app/lexanalyzer/postagger/action.h"
#include "app/lexanalyzer/postagger/score_context.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

Weight::Weight() {
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        cache.push_back( ctx.W0 ); }
        )
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        cache.push_back( ctx.W1 ); }
        )
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        cache.push_back( ctx.W2); }
        )
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        cache.push_back( ctx.W_1 ); }
        )
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        cache.push_back( ctx.W_2 ); }
        )
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        if (ctx.PRE1.size() > 0) { cache.push_back( ctx.PRE1 ); }
        })
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        if (ctx.PRE2.size() > 0) { cache.push_back( ctx.PRE2 ); }
        })
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        if (ctx.PRE3.size() > 0) { cache.push_back( ctx.PRE3 ); }
        })
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        if (ctx.PRE4.size() > 0) { cache.push_back( ctx.PRE4 ); }
        })
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        if (ctx.SUF1.size() > 0) { cache.push_back( ctx.SUF1 ); }
        })
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        if (ctx.SUF2.size() > 0) { cache.push_back( ctx.SUF2 ); }
        })
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        if (ctx.SUF3.size() > 0) { cache.push_back( ctx.SUF3 ); }
        })
      );
  repo.push_back( map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void {
        if (ctx.SUF4.size() > 0) { cache.push_back( ctx.SUF4 ); }
        })
      );
  ufeat_map_repo.push_back( uf_map_t(
        [](const ScoreContext& ctx, std::vector< ufp_t >& cache) -> void {
        cache.push_back( ufp_t(ctx.NUM ) );
        })
      );
  ufeat_map_repo.push_back( uf_map_t(
        [](const ScoreContext& ctx, std::vector< ufp_t >& cache) -> void {
        cache.push_back( ufp_t(ctx.UPPERCASE) );
        })
      );
  ufeat_map_repo.push_back( uf_map_t(
        [](const ScoreContext& ctx, std::vector< ufp_t >& cache) -> void {
        cache.push_back( ufp_t(ctx.HYPHEN) );
        })
      );
  ufeat_map_repo.push_back( uf_map_t(
        [](const ScoreContext& ctx, std::vector< ufp_t >& cache) -> void {
        cache.push_back( ufp_t(ctx.T_1) );
        })
      );
  bfeat_map_repo.push_back( bf_map_t(
        [](const ScoreContext& ctx, std::vector< bfp_t >& cache) -> void {
        cache.push_back( bfp_t(ctx.T_2, ctx.T_1) );
        })
      );
}

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
