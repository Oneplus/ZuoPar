#ifndef __ZUOPAR_MODEL_ASSOCIATED_POINTWISE_FEATURE_PARAM_MAP_COLLECTION_H__
#define __ZUOPAR_MODEL_ASSOCIATED_POINTWISE_FEATURE_PARAM_MAP_COLLECTION_H__

#include "model/meta_feature.h"
#include "feature_param_map.h"
#include <vector>
#include <boost/unordered_map.hpp>

namespace ZuoPar {

template <class _StateType,
          class _ScoreContextType,
          class _ActionType>
class FeaturePointwiseParameterCollection {
public:
  //!
  typedef UnigramMetaFeature  ufp_t;
  //!
  typedef BigramMetaFeature   bfp_t;
  //!
  typedef TrigramMetaFeature  tfp_t;
  //! Instantiate the unigram score type
  typedef Feature<UnigramMetaFeature, _ActionType> uf_t;
  //! Instantiate the bigram score type
  typedef Feature<BigramMetaFeature, _ActionType>  bf_t;
  //! Instantiate the trigram score type
  typedef Feature<TrigramMetaFeature, _ActionType> tf_t;
  //! Instantiate the unigram mapping
  typedef FeaturePointwiseParameterMap< ufp_t, _ScoreContextType, _ActionType > uf_map_t;
  //! Instantiate the bigram mapping
  typedef FeaturePointwiseParameterMap< bfp_t, _ScoreContextType, _ActionType > bf_map_t;
  //! Instantiate the trigram mapping
  typedef FeaturePointwiseParameterMap< tfp_t, _ScoreContextType, _ActionType > tf_map_t;

public:
  FeaturePointwiseParameterCollection() {}

  /**
   * Get score for the state.
   *
   *  @param[in]  state   The state
   *  @param[in]  act     The action
   *  @param[in]  avg     The average parameter.
   *  @return     floatval_t  The score of applying the action act to the state.
   */
  floatval_t score(const _StateType& state, const _ActionType& act, bool avg) {
    _ScoreContextType ctx(state);
    return score(ctx, act, avg);
  }

  /**
   * Get score for the state context.
   *
   *  @param[in]  ctx   The input state context.
   *  @param[in]  act   The action.
   *  @param[in]  avg   The average parameter.
   *  @return     floatval_t  The score of applying the action act to the state
   *                           context.
   */
  floatval_t score(const _ScoreContextType& ctx, const _ActionType& act, bool avg) {
    floatval_t ret = 0;
    for (int i = 0; i < ufeat_map_repo.size(); ++ i) {
      ret += ufeat_map_repo[i].score(ctx, act, avg, 0.);
    }

    for (int i = 0; i < bfeat_map_repo.size(); ++ i) {
      ret += bfeat_map_repo[i].score(ctx, act, avg, 0.);
    }

    for (int i = 0; i < tfeat_map_repo.size(); ++ i) {
      ret += tfeat_map_repo[i].score(ctx, act, avg, 0.);
    }
    return ret;
  }


  void batchly_score(const _ScoreContextType& ctx,
      const std::vector<_ActionType>& actions,
      bool avg,
      boost::unordered_map<_ActionType, floatval_t>& result) {
    for (int i = 0; i < ufeat_map_repo.size(); ++ i) {
      ufeat_map_repo[i].batchly_score(ctx, actions, avg, result);
    }

    for (int i = 0; i < bfeat_map_repo.size(); ++ i) {
      bfeat_map_repo[i].batchly_score(ctx, actions, avg, result);
    }

    for (int i = 0; i < tfeat_map_repo.size(); ++ i) {
      tfeat_map_repo[i].batchly_score(ctx, actions, avg, result);
    }
  }

  /**
   * Update the model with the state.
   *
   *  @param[in]  state     The input state.
   *  @param[in]  act       The action.
   *  @param[in]  timestamp The updated timestamp.
   *  @param[in]  scale     The updated scale.
   */
  void update(const _StateType& state, const _ActionType& act, int timestamp,
      floatval_t scale) {
    _ScoreContextType ctx(state);
    update(ctx, act, timestamp, scale);
  }

  /**
   * Update the model with the state context.
   *
   *  @param[in]  ctx       The input state context.
   *  @param[in]  act       The action.
   *  @param[in]  timestamp The updated timestamp.
   *  @param[in]  scale     The updated scale.
   */
  void update(const _ScoreContextType& ctx, const _ActionType& act, int timestamp,
      floatval_t scale) {
    for (int i = 0; i < ufeat_map_repo.size(); ++ i) {
      ufeat_map_repo[i].update(ctx, act, timestamp, scale);
    }

    for (int i = 0; i < bfeat_map_repo.size(); ++ i) {
      bfeat_map_repo[i].update(ctx, act, timestamp, scale);
    }

    for (int i = 0; i < tfeat_map_repo.size(); ++ i) {
      tfeat_map_repo[i].update(ctx, act, timestamp, scale);
    }
  }

  /**
   */
  void flush(int timestamp) {
    for (int i = 0; i < ufeat_map_repo.size(); ++ i) {
      ufeat_map_repo[i].flush(timestamp);
    }
    for (int i = 0; i < bfeat_map_repo.size(); ++ i) {
      bfeat_map_repo[i].flush(timestamp);
    }
    for (int i = 0; i < tfeat_map_repo.size(); ++ i) {
      tfeat_map_repo[i].flush(timestamp);
    }
  }

  /**
   * Save the model into the output stream.
   *
   *  @param[out] os    The output stream.
   *  @return     bool  If successfully saved, return true; otherwise return
   *                    false.
   */
  bool save(std::ostream& os) {
    boost::archive::text_oarchive oa(os);
    save(oa);
    return true;
  }

  bool save(boost::archive::text_oarchive& oa) {
    for (int i = 0; i < ufeat_map_repo.size(); ++ i) {
      ufeat_map_repo[i].save(oa);
    }
    for (int i = 0; i < bfeat_map_repo.size(); ++ i) {
      bfeat_map_repo[i].save(oa);
    }
    for (int i = 0; i < tfeat_map_repo.size(); ++ i) {
      tfeat_map_repo[i].save(oa);
    }
  }

  /**
   * Load the model fron input stream.
   *
   *  @param[in]  is    The input stream.
   *  @return     bool  If successfully loaded, return true; otherwise return
   *                    false.
   */
  bool load(std::istream& is) {
    boost::archive::text_iarchive ia(is);
    load(ia);
    return true;
  }

  bool load(boost::archive::text_iarchive& ia) {
    for (int i = 0; i < ufeat_map_repo.size(); ++ i) {
      ufeat_map_repo[i].load(ia);
    }

    for (int i = 0; i < bfeat_map_repo.size(); ++ i) {
      bfeat_map_repo[i].load(ia);
    }

    for (int i = 0; i < tfeat_map_repo.size(); ++ i) {
      tfeat_map_repo[i].load(ia);
    }
  }

protected:
  //! The unigram score mapping repository.
  std::vector< uf_map_t > ufeat_map_repo;
  //! The bigram score mapping repository.
  std::vector< bf_map_t > bfeat_map_repo;
  //! The trigram score mapping repository.
  std::vector< tf_map_t > tfeat_map_repo;

  //! The unigram score caching facility.
  std::vector< uf_t > ufeat_cache;
  //! The bigram score caching facility.
  std::vector< bf_t > bfeat_cache;
  //! The trigram score caching facility.
  std::vector< tf_t > tfeat_cache;
};

#define ZUOPAR_EXTRACTOR_U0(name) [](const ScoreContext& ctx, \
    std::vector<ufp_t>& cache) -> void{ \
  cache.push_back( ufp_t(ctx.name) ); \
}

#define ZUOPAR_EXTRACTOR_U1(name) [](const ScoreContext& ctx, \
    std::vector<ufp_t>& cache) -> void{ \
  if (ctx.name) { \
    cache.push_back( ufp_t(ctx.name) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_B00(name1, name2) [](const ScoreContext& ctx, \
    std::vector<bfp_t>& cache) -> void{ \
  cache.push_back( bfp_t( ctx.name1, ctx.name2) ); \
}

#define ZUOPAR_EXTRACTOR_B10(name1, name2) [](const ScoreContext& ctx, \
    std::vector<bfp_t>& cache) -> void{ \
  if (ctx.name1) { \
    cache.push_back( bfp_t( ctx.name1, ctx.name2) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_B10(name1, name2) [](const ScoreContext& ctx, \
    std::vector<bfp_t>& cache) -> void{ \
  if (ctx.name1) { \
    cache.push_back( bfp_t( ctx.name1, ctx.name2) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_B11(name1, name2) [](const ScoreContext& ctx, \
    std::vector<bfp_t>& cache) -> void{ \
  if (ctx.name1 && ctx.name2) { \
    cache.push_back( bfp_t( ctx.name1, ctx.name2) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_T000(name1, name2, name3) [](const ScoreContext& ctx, \
    std::vector<tfp_t>& cache) -> void{ \
  cache.push_back( tfp_t(ctx.name1, ctx.name2, ctx.name3) ); \
}

#define ZUOPAR_EXTRACTOR_T100(name1, name2, name3) [](const ScoreContext& ctx, \
    std::vector<tfp_t>& cache) -> void{ \
  if (ctx.name1) { \
    cache.push_back( tfp_t(ctx.name1, ctx.name2, ctx.name3) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_T110(name1, name2, name3) [](const ScoreContext& ctx, \
    std::vector<tfp_t>& cache) -> void{ \
  if (ctx.name1 && ctx.name2) { \
    cache.push_back( tfp_t(ctx.name1, ctx.name2, ctx.name3) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_T111(name1, name2, name3) [](const ScoreContext& ctx, \
    std::vector<tfp_t>& cache) -> void{ \
  if (ctx.name1 && ctx.name2 && ctx.name3) { \
    cache.push_back( tfp_t(ctx.name1, ctx.name2, ctx.name3) ); \
  } \
}

#define ZUOPAR_FEATURE_MAP_REGIST_U0(name) do { \
  ufeat_map_repo.push_back( uf_map_t( ZUOPAR_EXTRACTOR_U0(name) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_U1(name) do { \
  ufeat_map_repo.push_back( uf_map_t( ZUOPAR_EXTRACTOR_U1(name) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_B00(name1, name2) do { \
  bfeat_map_repo.push_back( bf_map_t( ZUOPAR_EXTRACTOR_B00(name1, name2) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_B10(name1, name2) do { \
  bfeat_map_repo.push_back( bf_map_t( ZUOPAR_EXTRACTOR_B10(name1, name2) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_B11(name1, name2) do { \
  bfeat_map_repo.push_back( bf_map_t( ZUOPAR_EXTRACTOR_B11(name1, name2) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_T000(name1, name2, name3) do { \
  tfeat_map_repo.push_back( tf_map_t( ZUOPAR_EXTRACTOR_T000(name1, name2, name3) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_T100(name1, name2, name3) do { \
  tfeat_map_repo.push_back( tf_map_t( ZUOPAR_EXTRACTOR_T100(name1, name2, name3) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_T110(name1, name2, name3) do { \
  tfeat_map_repo.push_back( tf_map_t( ZUOPAR_EXTRACTOR_T110(name1, name2, name3) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_T111(name1, name2, name3) do { \
  tfeat_map_repo.push_back( tf_map_t( ZUOPAR_EXTRACTOR_T111(name1, name2, name3) ) ); \
} while (0);

} //  end for zuopar

#endif  //  end for __ZUOPAR_MODEL_ASSOCIATED_POINTWISE_FEATURE_PARAM_MAP_COLLECTION_H__
