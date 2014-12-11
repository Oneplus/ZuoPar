#ifndef __ZUOPAR_MODEL_META_FEATURE_H__
#define __ZUOPAR_MODEL_META_FEATURE_H__

#include "settings.h"
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZuoPar {

class AbstractMetaFeature {
protected:
  std::size_t seed;
public:
  //! The default constructor.
  AbstractMetaFeature() : seed(0) {}
  friend class boost::serialization::access;
  //! hash value.
  friend std::size_t hash_value(const AbstractMetaFeature& s) {
    return s.seed;
  }
};

class UnigramMetaFeature: public AbstractMetaFeature {
private:
  int feat;

public:
  UnigramMetaFeature() { }
  UnigramMetaFeature(int f) : feat(f) {
    boost::hash_combine(seed, f);
  }

  //! The equal operator.
  bool operator == (const UnigramMetaFeature& a) const {
    return (a.feat == feat);
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & feat;
  }

  friend std::ostream& operator <<(std::ostream& os,
      const UnigramMetaFeature& s) {
    os << s.feat;
    return os;
  }
};

class BigramMetaFeature: public AbstractMetaFeature {
private:
  int feat0, feat1;
public:
  BigramMetaFeature() { }
  BigramMetaFeature(int f0, int f1): feat0(f0), feat1(f1) {
    boost::hash_combine(seed, f0);
    boost::hash_combine(seed, f1);
  }

  //! The equal operator.
  bool operator == (const BigramMetaFeature& a) const {
    return (a.feat0 == feat0 && a.feat1 == feat1);
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & feat0 & feat1;
  }

  friend std::ostream& operator <<(std::ostream& os,
      const BigramMetaFeature& s) {
    os << s.feat0 << "," << s.feat1;
    return os;
  }
};

class TrigramMetaFeature: public AbstractMetaFeature {
private:
  int feat0, feat1, feat2;
public:
  TrigramMetaFeature() { }
  TrigramMetaFeature(int f0, int f1, int f2) {
    boost::hash_combine(seed, f0);
    boost::hash_combine(seed, f1);
    boost::hash_combine(seed, f2);
  }

  //! The equal operator.
  bool operator == (const TrigramMetaFeature& a) const {
    return (a.feat0 == feat0 && a.feat1 == feat1 && a.feat2 == feat2);
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & feat0 & feat1 & feat2;
  }

  friend std::ostream& operator <<(std::ostream& os,
      const TrigramMetaFeature& s) {
    os << s.feat0 << "," << s.feat1 << "," << s.feat2;
    return os;
  }
};

}   //  end for namespace ZuoPar

#endif  //  end for __ZUOPAR_MODEL_META_FEATURE_H__
