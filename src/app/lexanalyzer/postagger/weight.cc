#include "app/lexanalyzer/postagger/weight.h"
#include "app/lexanalyzer/postagger/action.h"
#include "app/lexanalyzer/postagger/score_context.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

Weight::Weight() {
  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        cache.push_back( StringFeature(ctx.W0, act) ); })
      );

  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        cache.push_back( StringFeature(ctx.W1, act) ); })
      );

  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        cache.push_back( StringFeature(ctx.W2, act) ); })
      );

  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        cache.push_back( StringFeature(ctx.W_1, act) ); })
      );

  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        cache.push_back( StringFeature(ctx.W_2, act) ); })
      );

  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        if (ctx.PRE1.size() > 0) { cache.push_back( StringFeature(ctx.PRE1, act) ); }
        })
      );
  
  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        if (ctx.PRE2.size() > 0) { cache.push_back( StringFeature(ctx.PRE2, act) ); }
        })
      );

  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        if (ctx.PRE3.size() > 0) { cache.push_back( StringFeature(ctx.PRE3, act) ); }
        })
      );

  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        if (ctx.PRE4.size() > 0) { cache.push_back( StringFeature(ctx.PRE4, act) ); }
        })
      );

  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        if (ctx.SUF1.size() > 0) { cache.push_back( StringFeature(ctx.SUF1, act) ); }
        })
      );

  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        if (ctx.SUF2.size() > 0) { cache.push_back( StringFeature(ctx.SUF2, act) ); }
        })
      );

  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        if (ctx.SUF3.size() > 0) { cache.push_back( StringFeature(ctx.SUF3, act) ); }
        })
      );

  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        if (ctx.SUF4.size() > 0) { cache.push_back( StringFeature(ctx.SUF4, act) ); }
        })
      );

  UnigramFeatureCollection::repo.push_back(
      UnigramFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<UnigramFeature>& cache) -> void {
        cache.push_back( UnigramFeature(ctx.NUM, act) );
        })
      );

  UnigramFeatureCollection::repo.push_back(
      UnigramFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<UnigramFeature>& cache) -> void {
        cache.push_back( UnigramFeature(ctx.UPPERCASE, act) );
        })
      );

  UnigramFeatureCollection::repo.push_back(
      UnigramFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<UnigramFeature>& cache) -> void {
        cache.push_back( UnigramFeature(ctx.HYPHEN, act) );
        })
      );

  UnigramFeatureCollection::repo.push_back(
      UnigramFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<UnigramFeature>& cache) -> void {
        cache.push_back( UnigramFeature(ctx.T_1, act) );
        })
      );

  BigramFeatureCollection::repo.push_back(
      BigramFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<BigramFeature>& cache) -> void {
        cache.push_back( BigramFeature(BigramMetaFeature(ctx.T_2, ctx.T_1), act) );
        })
      );
}

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
