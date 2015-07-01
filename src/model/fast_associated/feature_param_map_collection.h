#ifndef __ZUOPAR_MODEL_FAST_ASSOCIATED_FEATURE_PARAM_MAP_COLLECTION_H__
#define __ZUOPAR_MODEL_FAST_ASSOCIATED_FEATURE_PARAM_MAP_COLLECTION_H__

#include "types/internal/packed_scores.h"
#include "types/math/sparse_vector.h"
#include "model/meta_feature.h"
#include "feature_param_map.h"
#include <vector>

namespace ZuoPar {

template <
  class _StateType,
  class _ScoreContextType,
  class _ActionType
>
class FeatureParameterCollection {
public:
  //! The unigram feature type
  typedef UnigramMetaFeature  ufp_t;

  //! The bigram feature type
  typedef BigramMetaFeature   bfp_t;

  //! The trigram feature type
  typedef TrigramMetaFeature  tfp_t;

  //! The quadgram feature type
  typedef QuadgramMetaFeature qfp_t;

  //! The trigram feature type
  typedef std::string  sfp_t;

  //! Instantiate the unigram mapping
  typedef FeatureParameterMap<
    ufp_t, _ScoreContextType, _ActionType
  > uf_map_t;

  //! Instantiate the bigram mapping
  typedef FeatureParameterMap<
    bfp_t, _ScoreContextType, _ActionType
  > bf_map_t;

  //! Instantiate the trigram mapping
  typedef FeatureParameterMap<
    tfp_t, _ScoreContextType, _ActionType
  > tf_map_t;

  //! Instantiate the quadgram mapping
  typedef FeatureParameterMap<
    qfp_t, _ScoreContextType, _ActionType
  > qf_map_t;

  //! Instantiate the string mapping
  typedef FeatureParameterMap<
    std::string, _ScoreContextType, _ActionType
  > sf_map_t;

  //! Define the packed score type.
  typedef PackedScores<_ActionType> packed_score_t;
public:
  FeatureParameterCollection() {}

  /**
   * Convert the pointwised feature collections into vector.
   *
   *  @param[in]  state         The state.
   *  @param[in]  act           The action
   *  @param[in]  scale         Increase the value in sparse_vector by scale.
   *  @param[out] sparse_vector The sparse vector.
   */
  void vectorize(const _StateType& state, const _ActionType& act, floatval_t scale,
      SparseVector* sparse_vector) {
    int global_id = 0;
    _ScoreContextType ctx(state);
    for (uf_map_t& entry: ufeat_map_repo) {
      entry.vectorize(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (bf_map_t& entry: bfeat_map_repo) {
      entry.vectorize(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (tf_map_t& entry: tfeat_map_repo) {
      entry.vectorize(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (qf_map_t& entry: qfeat_map_repo) {
      entry.vectorize(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (sf_map_t& entry: sfeat_map_repo) {
      entry.vectorize(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
  }

  /**
   * Convert the pointwised feature collections into vector.
   *
   *  @param[in]  state         The state
   *  @param[in]  act           The action
   *  @param[in]  scale         Increase the value in sparse_vector by scale.
   *  @param[out] sparse_vector The version 2 sparse vector.
   */
  void vectorize2(const _StateType& state, const _ActionType& act,
      const floatval_t& scale, SparseVector2* sparse_vector) {
    _ScoreContextType ctx(state);
    vectorize2(ctx, act, scale, sparse_vector);
  }

  void vectorize2(const _ScoreContextType& ctx, const _ActionType& act,
      const floatval_t& scale, SparseVector2* sparse_vector) {
    int global_id = 0;
    for (uf_map_t& entry: ufeat_map_repo) {
      entry.vectorize2(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (bf_map_t& entry: bfeat_map_repo) {
      entry.vectorize2(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (tf_map_t& entry: tfeat_map_repo) {
      entry.vectorize2(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (qf_map_t& entry: qfeat_map_repo) {
      entry.vectorize2(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (sf_map_t& entry: sfeat_map_repo) {
      entry.vectorize2(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
  }

  /**
   * Convert the pointwised feature collections into vector.
   *
   *  @param[in]  state         The state
   *  @param[in]  act           The action
   *  @param[in]  scale         Increase the value in sparse_vector by scale.
   *  @param[out] sparse_vector The version 3 sparse vector.
   */
  void vectorize3(const _StateType& state, const _ActionType& act,
      const floatval_t& scale, SparseVector3* sparse_vector) {
    _ScoreContextType ctx(state);
    vectorize3(ctx, act, scale, sparse_vector);
  }

  void vectorize3(const _ScoreContextType& ctx, const _ActionType& act,
      const floatval_t& scale, SparseVector3* sparse_vector) {
    int global_id = 0;
    for (uf_map_t& entry: ufeat_map_repo) {
      entry.vectorize3(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (bf_map_t& entry: bfeat_map_repo) {
      entry.vectorize3(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (tf_map_t& entry: tfeat_map_repo) {
      entry.vectorize3(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (qf_map_t& entry: qfeat_map_repo) {
      entry.vectorize3(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (sf_map_t& entry: sfeat_map_repo) {
      entry.vectorize3(ctx, act, scale, global_id, sparse_vector);
      ++ global_id;
    }
  }

  /**
   * Get score for the state.
   *
   *  @param[in]  state   The state
   *  @param[in]  act     The action
   *  @param[in]  avg     The average parameter.
   *  @return     floatval_t  The score of applying the action act to the state.
   */
  floatval_t score(const _StateType& state, const _ActionType& act,
      bool avg) const {
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
  floatval_t score(const _ScoreContextType& ctx, const _ActionType& act,
      bool avg) const {
    floatval_t ret = 0;
    for (const uf_map_t& entry: ufeat_map_repo) { ret += entry.score(ctx, act, avg); }
    for (const bf_map_t& entry: bfeat_map_repo) { ret += entry.score(ctx, act, avg); }
    for (const tf_map_t& entry: tfeat_map_repo) { ret += entry.score(ctx, act, avg); }
    for (const qf_map_t& entry: qfeat_map_repo) { ret += entry.score(ctx, act, avg); }
    for (const sf_map_t& entry: sfeat_map_repo) { ret += entry.score(ctx, act, avg); }
    return ret;
  }

  void batchly_score(const _StateType& state,
      const std::vector<_ActionType>& actions,
      bool avg,
      packed_score_t& result) const {
    _ScoreContextType ctx(state);
    return batchly_score(ctx, actions, avg, result);
  }

  void batchly_score(const _ScoreContextType& ctx,
      const std::vector<_ActionType>& actions,
      bool avg,
      packed_score_t& result) const {
    for (const uf_map_t& entry: ufeat_map_repo) { entry.batchly_score(ctx, actions, avg, result); }
    for (const bf_map_t& entry: bfeat_map_repo) { entry.batchly_score(ctx, actions, avg, result); }
    for (const tf_map_t& entry: tfeat_map_repo) { entry.batchly_score(ctx, actions, avg, result); }
    for (const qf_map_t& entry: qfeat_map_repo) { entry.batchly_score(ctx, actions, avg, result); }
    for (const sf_map_t& entry: sfeat_map_repo) { entry.batchly_score(ctx, actions, avg, result); }
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
      const floatval_t& scale) {
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
      const floatval_t& scale) {
    for (uf_map_t& entry: ufeat_map_repo) { entry.update(ctx, act, timestamp, scale); }
    for (bf_map_t& entry: bfeat_map_repo) { entry.update(ctx, act, timestamp, scale); }
    for (tf_map_t& entry: tfeat_map_repo) { entry.update(ctx, act, timestamp, scale); }
    for (qf_map_t& entry: qfeat_map_repo) { entry.update(ctx, act, timestamp, scale); }
    for (sf_map_t& entry: sfeat_map_repo) { entry.update(ctx, act, timestamp, scale); }
  }

  /**
   * Flush the parameter.
   *
   *  @param[in]  timestamp   The time stamp.
   */
  void flush(int timestamp) {
    for (uf_map_t& entry: ufeat_map_repo) { entry.flush(timestamp); }
    for (bf_map_t& entry: bfeat_map_repo) { entry.flush(timestamp); }
    for (tf_map_t& entry: tfeat_map_repo) { entry.flush(timestamp); }
    for (qf_map_t& entry: qfeat_map_repo) { entry.flush(timestamp); }
    for (sf_map_t& entry: sfeat_map_repo) { entry.flush(timestamp); }
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
    for (uf_map_t& entry: ufeat_map_repo) { entry.save(oa); }
    for (bf_map_t& entry: bfeat_map_repo) { entry.save(oa); }
    for (tf_map_t& entry: tfeat_map_repo) { entry.save(oa); }
    for (qf_map_t& entry: qfeat_map_repo) { entry.save(oa); }
    for (sf_map_t& entry: sfeat_map_repo) { entry.save(oa); }
    return true;
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
    for (uf_map_t& entry: ufeat_map_repo) { entry.load(ia); }
    for (bf_map_t& entry: bfeat_map_repo) { entry.load(ia); }
    for (tf_map_t& entry: tfeat_map_repo) { entry.load(ia); }
    for (qf_map_t& entry: qfeat_map_repo) { entry.load(ia); }
    for (sf_map_t& entry: sfeat_map_repo) { entry.load(ia); }
    return true;
  }

protected:
  std::vector< uf_map_t > ufeat_map_repo;  //! The unigram score mapping repository.
  std::vector< bf_map_t > bfeat_map_repo;  //! The bigram score mapping repository.
  std::vector< tf_map_t > tfeat_map_repo;  //! The trigram score mapping repository.
  std::vector< qf_map_t > qfeat_map_repo;  //! The quadgram score mapping repository.
  std::vector< sf_map_t > sfeat_map_repo;  //! The string score mapping repository.
};

#define ZUOPAR_EXTRACTOR_U0(_1) [](const ScoreContext& ctx, \
    std::vector<ufp_t>& cache) -> void{ \
  cache.push_back( ufp_t(ctx._1) ); \
}

#define ZUOPAR_EXTRACTOR_U1(_1) [](const ScoreContext& ctx, \
    std::vector<ufp_t>& cache) -> void{ \
  if (ctx._1) { \
    cache.push_back( ufp_t(ctx._1) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_B00(_1, _2) [](const ScoreContext& ctx, \
    std::vector<bfp_t>& cache) -> void{ \
  cache.push_back( bfp_t( ctx._1, ctx._2) ); \
}

#define ZUOPAR_EXTRACTOR_B10(_1, _2) [](const ScoreContext& ctx, \
    std::vector<bfp_t>& cache) -> void{ \
  if (ctx._1) { \
    cache.push_back( bfp_t( ctx._1, ctx._2) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_B11(_1, _2) [](const ScoreContext& ctx, \
    std::vector<bfp_t>& cache) -> void{ \
  if (ctx._1 && ctx._2) { \
    cache.push_back( bfp_t( ctx._1, ctx._2) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_T000(_1, _2, _3) [](const ScoreContext& ctx, \
    std::vector<tfp_t>& cache) -> void{ \
  cache.push_back( tfp_t(ctx._1, ctx._2, ctx._3) ); \
}

#define ZUOPAR_EXTRACTOR_T100(_1, _2, _3) [](const ScoreContext& ctx, \
    std::vector<tfp_t>& cache) -> void{ \
  if (ctx._1) { \
    cache.push_back( tfp_t(ctx._1, ctx._2, ctx._3) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_T110(_1, _2, _3) [](const ScoreContext& ctx, \
    std::vector<tfp_t>& cache) -> void{ \
  if (ctx._1 && ctx._2) { \
    cache.push_back( tfp_t(ctx._1, ctx._2, ctx._3) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_T111(_1, _2, _3) [](const ScoreContext& ctx, \
    std::vector<tfp_t>& cache) -> void{ \
  if (ctx._1 && ctx._2 && ctx._3) { \
    cache.push_back( tfp_t(ctx._1, ctx._2, ctx._3) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_Q1100(_1, _2, _3, _4) [](const ScoreContext& ctx, \
    std::vector<qfp_t>& cache) -> void{ \
  if (ctx._1 && ctx._2) { \
    cache.push_back( qfp_t(ctx._1, ctx._2, ctx._3, ctx._4) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_Q1111(_1, _2, _3, _4) [](const ScoreContext& ctx, \
    std::vector<qfp_t>& cache) -> void{ \
  if (ctx._1 && ctx._2 && ctx._3 && ctx._4) { \
    cache.push_back( qfp_t(ctx._1, ctx._2, ctx._3, ctx._4) ); \
  } \
}

#define ZUOPAR_EXTRACTOR_S0(_1) [](const ScoreContext& ctx, \
    std::vector<sfp_t>& cache) -> void{ \
  cache.push_back( sfp_t(ctx._1) ); \
}

#define ZUOPAR_EXTRACTOR_S1(_1) [](const ScoreContext& ctx, \
    std::vector<sfp_t>& cache) -> void{ \
  if (ctx._1.size() > 0) { \
    cache.push_back( sfp_t(ctx._1) ); \
  } \
}


#define ZUOPAR_FEATURE_MAP_REGIST_U0(_1) do { \
  ufeat_map_repo.push_back( uf_map_t( ZUOPAR_EXTRACTOR_U0(_1) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_U1(_1) do { \
  ufeat_map_repo.push_back( uf_map_t( ZUOPAR_EXTRACTOR_U1(_1) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_B00(_1, _2) do { \
  bfeat_map_repo.push_back( bf_map_t( ZUOPAR_EXTRACTOR_B00(_1, _2) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_B10(_1, _2) do { \
  bfeat_map_repo.push_back( bf_map_t( ZUOPAR_EXTRACTOR_B10(_1, _2) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_B11(_1, _2) do { \
  bfeat_map_repo.push_back( bf_map_t( ZUOPAR_EXTRACTOR_B11(_1, _2) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_T000(_1, _2, _3) do { \
  tfeat_map_repo.push_back( tf_map_t( ZUOPAR_EXTRACTOR_T000(_1, _2, _3) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_T100(_1, _2, _3) do { \
  tfeat_map_repo.push_back( tf_map_t( ZUOPAR_EXTRACTOR_T100(_1, _2, _3) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_T110(_1, _2, _3) do { \
  tfeat_map_repo.push_back( tf_map_t( ZUOPAR_EXTRACTOR_T110(_1, _2, _3) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_T111(_1, _2, _3) do { \
  tfeat_map_repo.push_back( tf_map_t( ZUOPAR_EXTRACTOR_T111(_1, _2, _3) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_Q1100(_1, _2, _3, _4) do { \
  qfeat_map_repo.push_back( qf_map_t( ZUOPAR_EXTRACTOR_Q1100(_1, _2, _3, _4) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_Q1111(_1, _2, _3, _4) do { \
  qfeat_map_repo.push_back( qf_map_t( ZUOPAR_EXTRACTOR_Q1111(_1, _2, _3, _4) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_S0(_1) do { \
  sfeat_map_repo.push_back( sf_map_t( ZUOPAR_EXTRACTOR_S0(_1) ) ); \
} while (0);

#define ZUOPAR_FEATURE_MAP_REGIST_S1(_1) do { \
  sfeat_map_repo.push_back( sf_map_t( ZUOPAR_EXTRACTOR_S1(_1) ) ); \
} while (0);

} //  end for zuopar

#endif  //  end for __ZUOPAR_MODEL_FAST_ASSOCIATED_FEATURE_PARAM_MAP_COLLECTION_H__
