#ifndef __ZUOPAR_MODEL_META_FEATURE_H__
#define __ZUOPAR_MODEL_META_FEATURE_H__

#include "settings.h"
#include <boost/functional/hash.hpp>
#include <boost/serialization/access.hpp>

namespace ZuoPar {

class AbstractMetaFeature {
protected:
  std::size_t seed;
  friend class boost::serialization::access;
public:
  //! The default constructor.
  AbstractMetaFeature(): seed(0) {}
  //! hash value.
  friend std::size_t hash_value(const AbstractMetaFeature& s) {
    return s.seed;
  }
};

class UnigramMetaFeature: public AbstractMetaFeature {
private:
  int feat;
public:
  UnigramMetaFeature(): feat(0) { }
  UnigramMetaFeature(const int& f): feat(f) { seed = f; /*boost::hash_combine(seed, f);*/ }

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
  BigramMetaFeature(): feat0(0), feat1(0) { }
  BigramMetaFeature(const int& f0, const int& f1): feat0(f0), feat1(f1) {
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
    os << "(" << s.feat0 << "," << s.feat1 << ")";
    return os;
  }
};

class TrigramMetaFeature: public AbstractMetaFeature {
private:
  int feat0, feat1, feat2;
public:
  TrigramMetaFeature(): feat0(0), feat1(0), feat2(0) { }
  TrigramMetaFeature(const int& f0, const int& f1, const int& f2)
    : feat0(f0), feat1(f1), feat2(f2) {
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
    os << "(" << s.feat0 << "," << s.feat1 << "," << s.feat2 << ")";
    return os;
  }
};

class QuadgramMetaFeature: public AbstractMetaFeature {
private:
  int feat0, feat1, feat2, feat3;
public:
  QuadgramMetaFeature(): feat0(0), feat1(0), feat2(0), feat3(0) { }
  QuadgramMetaFeature(const int& f0, const int& f1, const int& f2, const int& f3)
    : feat0(f0), feat1(f1), feat2(f2), feat3(f3) {
    boost::hash_combine(seed, f0);
    boost::hash_combine(seed, f1);
    boost::hash_combine(seed, f2);
    boost::hash_combine(seed, f3);
  }

  //! The equal operator.
  bool operator == (const QuadgramMetaFeature& a) const {
    return (a.feat0 == feat0 && a.feat1 == feat1 && a.feat2 == feat2 && a.feat3 == feat3);
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & feat0 & feat1 & feat2 & feat3;
  }

  friend std::ostream& operator <<(std::ostream& os,
      const QuadgramMetaFeature& s) {
    os << "(" << s.feat0 << "," << s.feat1 << "," << s.feat2 << "," << s.feat3 << ")";
    return os;
  }
};

class QuingramMetaFeature: public AbstractMetaFeature {
private:
  int feat0, feat1, feat2, feat3, feat4;
public:
  QuingramMetaFeature(): feat0(0), feat1(0), feat2(0), feat3(0), feat4(0) {}
  QuingramMetaFeature(const int& f0, const int& f1, const int& f2, const int& f3,
      const int& f4)
    : feat0(f0), feat1(f1), feat2(f2), feat3(f3), feat4(f4) {
    boost::hash_combine(seed, f0);
    boost::hash_combine(seed, f1);
    boost::hash_combine(seed, f2);
    boost::hash_combine(seed, f3);
    boost::hash_combine(seed, f4);
  }

  //! The equal operator.
  bool operator == (const QuingramMetaFeature& a) const {
    return (a.feat0 == feat0 && a.feat1 == feat1
        && a.feat2 == feat2 && a.feat3 == feat3
        && a.feat4 == feat4);
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & feat0 & feat1 & feat2 & feat3 & feat4;
  }

  friend std::ostream& operator <<(std::ostream& os,
      const QuingramMetaFeature& s) {
    os << "(" << s.feat0 << "," << s.feat1 << "," << s.feat2 << ","
      << s.feat3 << "," << s.feat4 << ")";
    return os;
  }
};

class HexgramMetaFeature: public AbstractMetaFeature {
private:
  int feat0, feat1, feat2, feat3, feat4, feat5;
public:
  HexgramMetaFeature()
    : feat0(0), feat1(0), feat2(0), feat3(0), feat4(0), feat5(0) {}
  HexgramMetaFeature(const int& f0, const int& f1, const int& f2, const int& f3,
      const int& f4, const int& f5)
    : feat0(f0), feat1(f1), feat2(f2), feat3(f3), feat4(f4), feat5(f5) {
    boost::hash_combine(seed, f0);
    boost::hash_combine(seed, f1);
    boost::hash_combine(seed, f2);
    boost::hash_combine(seed, f3);
    boost::hash_combine(seed, f4);
    boost::hash_combine(seed, f5);
  }

  //! The equal operator.
  bool operator == (const HexgramMetaFeature& a) const {
    return (a.feat0 == feat0 && a.feat1 == feat1
        && a.feat2 == feat2 && a.feat3 == feat3
        && a.feat4 == feat4 && a.feat5 == feat5);
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & feat0 & feat1 & feat2 & feat3 & feat4 & feat5;
  }

  friend std::ostream& operator <<(std::ostream& os,
      const HexgramMetaFeature& s) {
    os << "(" << s.feat0 << "," << s.feat1 << "," << s.feat2 << ","
      << s.feat3 << "," << s.feat4 << "," << s.feat5 << ")";
    return os;
  }
};

}   //  end for namespace ZuoPar

#endif  //  end for __ZUOPAR_MODEL_META_FEATURE_H__
