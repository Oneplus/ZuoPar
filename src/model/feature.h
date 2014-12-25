#ifndef __ZUOPAR_MODEL_FEATURE_H__
#define __ZUOPAR_MODEL_FEATURE_H__

#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZuoPar {

template <class _MetaFeatureType, class _ActionType>
class Feature {
private:
  //!
  std::size_t seed;
  //! The hash seed for meta, used in replace.
  std::size_t seed1;
  //!
  _MetaFeatureType meta;
  //!
  _ActionType act;
public:
  Feature(): seed(0) {}
  Feature(const _MetaFeatureType& meta_, const _ActionType& act_)
    : meta(meta_), act(act_), seed(0) {
    boost::hash_combine(seed, meta_);
    seed1 = seed;
    boost::hash_combine(seed, act_);
  }

  void replace_action(const _ActionType& act_) {
    act = act_;
    seed = seed1;
    boost::hash_combine(seed, act);
  }

  void set(const _MetaFeatureType& meta_,
      const _ActionType& act_) {
    meta = meta_;
    act = act_;
    seed = 0;
    boost::hash_combine(seed, meta_);
    seed1 = seed;
    boost::hash_combine(seed, act_);
  }

  //! The equal operator.
  bool operator == (const Feature<_MetaFeatureType, _ActionType>& a) const {
    return (a.meta == meta && a.act == act);
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & meta & act;
  }

  friend std::size_t hash_value(const Feature<_MetaFeatureType, _ActionType>& s) {
    return s.seed;
  }

  friend std::ostream& operator <<(std::ostream& os,
      const Feature<_MetaFeatureType, _ActionType>& s) {
    os << "<" << s.meta << ", " << s.act << ">";
    return os;
  }
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_MODEL_FEATURE_H__
