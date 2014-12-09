#ifndef __ZUOPAR_MODEL_FEATURE_H__
#define __ZUOPAR_MODEL_FEATURE_H__

#include "settings.h"
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZuoPar {

class AbstractFeaturePrefix {
protected:
  std::size_t seed;
public:
  AbstractFeaturePrefix() : seed(0) {}
  friend class boost::serialization::access;
  friend std::size_t hash_value(const AbstractFeaturePrefix& s) {
    return s.seed;
  }
};

class UnigramFeaturePrefix: public AbstractFeaturePrefix {
private:
  int feat;

public:
  UnigramFeaturePrefix() { }
  UnigramFeaturePrefix(int f) : feat(f) {
    boost::hash_combine(seed, f);
  }

  //! The equal operator.
  bool operator == (const UnigramFeaturePrefix& a) const {
    return (a.seed == seed && a.feat == feat);
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & feat;
  }

  friend std::ostream& operator <<(std::ostream& os,
      const UnigramFeaturePrefix& s) {
    os << s.feat;
    return os;
  }
};

class BigramFeaturePrefix: public AbstractFeaturePrefix {
private:
  int feat0, feat1;
public:
  BigramFeaturePrefix() { }
  BigramFeaturePrefix(int f0, int f1): feat0(f0), feat1(f1) {
    boost::hash_combine(seed, f0);
    boost::hash_combine(seed, f1);
  }

  //! The equal operator.
  bool operator == (const BigramFeaturePrefix& a) const {
    return (a.seed  == seed && a.feat0 == feat0 && a.feat1 == feat1);
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & feat0 & feat1;
  }

  friend std::ostream& operator <<(std::ostream& os,
      const BigramFeaturePrefix& s) {
    os << s.feat0 << "," << s.feat1;
    return os;
  }
};

class TrigramFeaturePrefix: public AbstractFeaturePrefix {
private:
  int feat0, feat1, feat2;
public:
  TrigramFeaturePrefix() { }
  TrigramFeaturePrefix(int f0, int f1, int f2) {
    boost::hash_combine(seed, f0);
    boost::hash_combine(seed, f1);
    boost::hash_combine(seed, f2);
  }

  //! The equal operator.
  bool operator == (const TrigramFeaturePrefix& a) const {
    return (a.seed  == seed && a.feat0 == feat0 && a.feat1 == feat1
        && a.feat2 == feat2);
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & feat0 & feat1 & feat2;
  }

  friend std::ostream& operator <<(std::ostream& os,
      const TrigramFeaturePrefix& s) {
    os << s.feat0 << "," << s.feat1 << "," << s.feat2;
    return os;
  }
};

template <class _CodeType, class _ActionType>
class Feature {
private:
  std::size_t seed;
  std::size_t seed1;
  _CodeType   code;
  _ActionType act;
public:
  Feature(): seed(0) {}
  Feature(const _CodeType& code_, const _ActionType& act_)
    : code(code_), act(act_), seed(0) {
    boost::hash_combine(seed, code_);
    seed1 = seed;
    boost::hash_combine(seed, act_);
  }

  void replace_action(const _ActionType& act_) {
    act = act_;
    seed = seed1;
    boost::hash_combine(seed, act);
  }

  //! The equal operator.
  bool operator == (const Feature<_CodeType, _ActionType>& a) const {
    return (a.seed == seed && a.code == code && a.act == act);
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & code & act;
  }

  friend std::size_t hash_value(const Feature<_CodeType, _ActionType>& s) {
    return s.seed;
  }

  friend std::ostream& operator <<(std::ostream& os,
      const Feature<_CodeType, _ActionType>& s) {
    os << "<" << s.code << ", " << s.act << ">";
    return os;
  }
};

}   //  end for namespace ZuoPar

#endif  //  end for __ZUOPAR_MODEL_FEATURE_H__
