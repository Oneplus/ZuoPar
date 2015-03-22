#include "app/lexanalyzer/wordseg/weight.h"
#include "app/lexanalyzer/wordseg/action_utils.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

Weight::Weight() {
  StringFeatureCollectionImpl1::repo.push_back(
      StringFeatureMapImpl1([](const ScoreContext& ctx,
          std::vector<std::string>& cache) -> void {
        cache.push_back( ctx.prev_ch ); })
      );
  StringFeatureCollectionImpl1::repo.push_back(
      StringFeatureMapImpl1([](const ScoreContext& ctx,
          std::vector<std::string>& cache) -> void {
        cache.push_back( ctx.curr_ch ); })
      );
  StringFeatureCollectionImpl1::repo.push_back(
      StringFeatureMapImpl1([](const ScoreContext& ctx,
          std::vector<std::string>& cache) -> void {
        cache.push_back( ctx.next_ch ); })
      );
  StringFeatureCollectionImpl1::repo.push_back(
      StringFeatureMapImpl1([](const ScoreContext& ctx,
          std::vector<std::string>& cache) -> void {
        cache.push_back( ctx.prev_ch + "/" + ctx.curr_ch ); })
      );
  StringFeatureCollectionImpl1::repo.push_back(
      StringFeatureMapImpl1([](const ScoreContext& ctx,
          std::vector<std::string>& cache) -> void {
        cache.push_back( ctx.curr_ch + "/" + ctx.next_ch ); })
      );
  StringFeatureCollectionImpl1::repo.push_back(
      StringFeatureMapImpl1([](const ScoreContext& ctx,
          std::vector<std::string>& cache) -> void {
        cache.push_back( ctx.prev_ch + "/" + ctx.curr_ch + "/" + ctx.next_ch ); })
      );
  StringFeatureCollectionImpl2::repo.push_back(
      StringFeatureMapImpl2([](const ScoreContext& ctx,
          const Action& act, std::vector<StringFeature>& cache) -> void {
        if (ActionUtils::is_split(act)) { cache.push_back(StringFeature(ctx.curr_w, act)); }
        })
      );
  StringFeatureCollectionImpl2::repo.push_back(
      StringFeatureMapImpl2([](const ScoreContext& ctx,
          const Action& act, std::vector<StringFeature>& cache) -> void {
        if (ActionUtils::is_split(act) && ctx.curr_w_len == 1) {
          cache.push_back(StringFeature("single", act)); }
        })
      );
  StringFeatureCollectionImpl2::repo.push_back(
      StringFeatureMapImpl2([](const ScoreContext& ctx,
          const Action& act, std::vector<StringFeature>& cache) -> void {
        if (ActionUtils::is_split(act) && ctx.curr_w_len > 1) {
          cache.push_back(StringFeature(ctx.curr_w_first + "/" + ctx.curr_w_last, act));
          cache.push_back(StringFeature(ctx.curr_w_first + "/" + ctx.curr_w_len_s,  act));
          cache.push_back(StringFeature(ctx.curr_w_last + "/" + ctx.curr_w_len_s,  act));
        }
        })
      );
  StringFeatureCollectionImpl2::repo.push_back(
      StringFeatureMapImpl2([](const ScoreContext& ctx,
          const Action& act, std::vector<StringFeature>& cache) -> void {
        if (ActionUtils::is_split(act) && ctx.prev_w_len != -1) {
          cache.push_back(StringFeature(ctx.curr_w + "/" + ctx.prev_w, act));
          cache.push_back(StringFeature(ctx.prev_w_last + "/" + ctx.curr_w,  act));
          cache.push_back(StringFeature(ctx.prev_w + "/" + ctx.curr_w_first,  act));
          cache.push_back(StringFeature(ctx.prev_w + "/" + ctx.curr_w_len_s, act));
          cache.push_back(StringFeature(ctx.curr_w + "/" + ctx.prev_w_len_s, act));
        }
        })
      );
}

void Weight::vectorize(const ScoreContext& ctx, const Action& act, floatval_t scale,
    SparseVector* sparse_vector) {
}

void Weight::vectorize2(const ScoreContext& ctx, const Action& act, floatval_t scale,
    SparseVector2* sparse_vector) {
}

void Weight::vectorize3(const ScoreContext& ctx, const Action& act, floatval_t scale,
    SparseVector3* sparse_vector) {
}

floatval_t Weight::score(const ScoreContext& ctx, const Action& act,
    bool avg) const {
  floatval_t ret = 0;
  ret += StringFeatureCollectionImpl1::score(ctx, act, avg);
  ret += StringFeatureCollectionImpl2::score(ctx, act, avg);
  return ret;
}

void Weight::batchly_score(const ScoreContext& ctx, const std::vector<Action>& actions,
    bool avg, PackedScores<Action>& result) const {
  StringFeatureCollectionImpl1::batchly_score(ctx, actions, avg, result);
  StringFeatureCollectionImpl2::batchly_score(ctx, actions, avg, result);
}

void Weight::update(const ScoreContext& ctx, const Action& act, int timestamp,
    floatval_t scale) {
  StringFeatureCollectionImpl1::update(ctx, act, timestamp, scale);
  StringFeatureCollectionImpl2::update(ctx, act, timestamp, scale);
}

void Weight::flush(int timestamp) {
  StringFeatureCollectionImpl1::flush(timestamp);
  StringFeatureCollectionImpl2::flush(timestamp);
}

bool Weight::save(std::ostream& os) const {
  StringFeatureCollectionImpl1::save(os);
  StringFeatureCollectionImpl2::save(os);
  return true;
}

bool Weight::load(std::istream& is) {
  StringFeatureCollectionImpl1::load(is);
  StringFeatureCollectionImpl2::load(is);
  return true;
}

} //  namespace chinesewordsegmentor
} //  namespace lexicalanalyzer
} //  namespace zuopar
