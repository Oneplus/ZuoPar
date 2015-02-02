#ifndef __ZUOPAR_MODEL_FAST_META_META_FEATURE_PARAM_MAP_COLLECTION_H__
#define __ZUOPAR_MODEL_FAST_META_META_FEATURE_PARAM_MAP_COLLECTION_H__

#include "types/internal/packed_scores.h"
#include "types/math/sparse_vector.h"
#include "model/meta_feature.h"
#include "meta_feature_param_map.h"
#include <vector>

namespace ZuoPar {

template <
  class _StateType,
  class _ScoreContextType
>
class MetaFeatureParameterCollection {
public:
  //! The unigram feature type
  typedef UnigramMetaFeature  uf_t;

  //! The bigram feature type
  typedef BigramMetaFeature   bf_t;

  //! The trigram feature type
  typedef TrigramMetaFeature  tf_t;

  //! The quadgram feature type
  typedef QuadgramMetaFeature qf_t;

  typedef MetaFeatureParameterMap<UnigramMetaFeature,  _ScoreContextType> uf_map_t;
  typedef MetaFeatureParameterMap<BigramMetaFeature,   _ScoreContextType> bf_map_t;
  typedef MetaFeatureParameterMap<TrigramMetaFeature,  _ScoreContextType> tf_map_t;
  typedef MetaFeatureParameterMap<QuadgramMetaFeature, _ScoreContextType> qf_map_t;
  typedef MetaFeatureParameterMap<std::string,         _ScoreContextType> sf_map_t;

  MetaFeatureParameterCollection() {}

  /**
   * Convert the pointwised feature collections into vector.
   *
   *  @param[in]  ctx           The score context
   *  @param[in]  scale         Increase the value in sparse_vector by scale.
   *  @param[out] sparse_vector The sparse vector.
   */
  void vectorize(const _ScoreContextType& ctx, floatval_t scale,
      SparseVector* sparse_vector) {
    int global_id = 0;
    for (uf_map_t& repo: ufeat_map_repo) {
      repo.vectorize(ctx, scale, global_id, sparse_vector);
      global_id += repo.size();
    }
    for (bf_map_t& repo: bfeat_map_repo) {
      repo.vectorize(ctx, scale, global_id, sparse_vector);
      global_id += repo.size();
    }
    for (tf_map_t& repo: tfeat_map_repo) {
      repo.vectorize(ctx, scale, global_id, sparse_vector);
      global_id += repo.size();
    }
    for (qf_map_t& repo: qfeat_map_repo) {
      repo.vectorize(ctx, scale, global_id, sparse_vector);
      global_id += repo.size();
    }
    for (sf_map_t& repo: sfeat_map_repo) {
      repo.vectorize(ctx, scale, global_id, sparse_vector);
      global_id += repo.size();
    }
  }

  /**
   * Convert the pointwised feature collections into vector.
   *
   *  @param[in]  ctx           The score context
   *  @param[in]  act           The action
   *  @param[in]  scale         Increase the value in sparse_vector by scale.
   *  @param[out] sparse_vector The version 2 sparse vector.
   */
  void vectorize2(const _ScoreContextType& ctx, floatval_t scale,
      SparseVector2* sparse_vector) {
    int global_id = 0;
    for (uf_map_t& repo: ufeat_map_repo) {
      repo.vectorize2(ctx, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (bf_map_t& repo: bfeat_map_repo) {
      repo.vectorize2(ctx, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (tf_map_t& repo: tfeat_map_repo) {
      repo.vectorize2(ctx, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (qf_map_t& repo: qfeat_map_repo) {
      repo.vectorize2(ctx, scale, global_id, sparse_vector);
      ++ global_id;
    }
    for (sf_map_t& repo: sfeat_map_repo) {
      repo.vectorize2(ctx, scale, global_id, sparse_vector);
      ++ global_id;
    }
  }

  /**
   * Get score for the state context.
   *
   *  @param[in]  ctx   The input state context.
   *  @param[in]  avg   The average parameter.
   *  @return     floatval_t  The score of applying the action act to the state
   *                           context.
   */
  floatval_t score(const _ScoreContextType& ctx, bool avg) {
    floatval_t ret = 0;
    for (uf_map_t& repo: ufeat_map_repo) { ret += repo.score(ctx, avg, 0.); }
    for (bf_map_t& repo: bfeat_map_repo) { ret += repo.score(ctx, avg, 0.); }
    for (tf_map_t& repo: tfeat_map_repo) { ret += repo.score(ctx, avg, 0.); }
    for (qf_map_t& repo: qfeat_map_repo) { ret += repo.score(ctx, avg, 0.); }
    for (sf_map_t& repo: sfeat_map_repo) { ret += repo.score(ctx, avg, 0.); }
    return ret;
  }

  /**
   * Update the model with the state context.
   *
   *  @param[in]  ctx       The input state context.
   *  @param[in]  act       The action.
   *  @param[in]  timestamp The updated timestamp.
   *  @param[in]  scale     The updated scale.
   */
  void update(const _ScoreContextType& ctx, int timestamp, floatval_t scale) {
    for (uf_map_t& repo: ufeat_map_repo) { repo.update(ctx, timestamp, scale); }
    for (bf_map_t& repo: bfeat_map_repo) { repo.update(ctx, timestamp, scale); }
    for (tf_map_t& repo: tfeat_map_repo) { repo.update(ctx, timestamp, scale); }
    for (qf_map_t& repo: qfeat_map_repo) { repo.update(ctx, timestamp, scale); }
    for (sf_map_t& repo: sfeat_map_repo) { repo.update(ctx, timestamp, scale); }
  }

  /**
   * flush the score.
   */
  void flush(int timestamp) {
    for (uf_map_t& repo: ufeat_map_repo) { repo.flush(timestamp); }
    for (bf_map_t& repo: bfeat_map_repo) { repo.flush(timestamp); }
    for (tf_map_t& repo: tfeat_map_repo) { repo.flush(timestamp); }
    for (qf_map_t& repo: qfeat_map_repo) { repo.flush(timestamp); }
    for (sf_map_t& repo: sfeat_map_repo) { repo.flush(timestamp); }
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
    for (uf_map_t& repo: ufeat_map_repo) { repo.save(oa); }
    for (bf_map_t& repo: bfeat_map_repo) { repo.save(oa); }
    for (tf_map_t& repo: tfeat_map_repo) { repo.save(oa); }
    for (qf_map_t& repo: qfeat_map_repo) { repo.save(oa); }
    for (sf_map_t& repo: sfeat_map_repo) { repo.save(oa); }
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
    for (uf_map_t& repo: ufeat_map_repo) { repo.load(ia); }
    for (bf_map_t& repo: bfeat_map_repo) { repo.load(ia); }
    for (tf_map_t& repo: tfeat_map_repo) { repo.load(ia); }
    for (qf_map_t& repo: qfeat_map_repo) { repo.load(ia); }
    for (sf_map_t& repo: sfeat_map_repo) { repo.load(ia); }
    return true;
  }

protected:
  std::vector< uf_map_t > ufeat_map_repo;  //! The unigram score mapping repository.
  std::vector< bf_map_t > bfeat_map_repo;  //! The bigram score mapping repository.
  std::vector< tf_map_t > tfeat_map_repo;  //! The trigram score mapping repository.
  std::vector< qf_map_t > qfeat_map_repo;  //! The quadgram score mapping repository.
  std::vector< sf_map_t > sfeat_map_repo;  //! The string score mapping repository.
};

} //  end for zuopar

#endif  //  end for __ZUOPAR_MODEL_FAST_META_META_FEATURE_PARAM_MAP_COLLECTION_H__