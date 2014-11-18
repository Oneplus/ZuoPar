#ifndef __ZUOPAR_MODEL_FEATURE_H__
#define __ZUOPAR_MODEL_FEATURE_H__

#include "settings.h"
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZuoPar {

//! Score is a kind of (code, action) tuple which representing the a feature.
//! Unigram score type.
template <class _ActionType>
class UnigramFeature {
private:
  boost::tuples::tuple<int, _ActionType> payload;

public:
  //! The empty default allocator
  UnigramFeature() {}

  //! The allocator with feature and action
  UnigramFeature(int feat, const _ActionType& act)
    : payload(feat, act) {
  }

  friend class boost::serialization::access;

  //! The equal operator.
  bool operator == (const UnigramFeature<_ActionType>& a) const {
    return (
        a.payload.template get<0>() == payload.template get<0>() &&
        a.payload.template get<1>() == payload.template get<1>());
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & payload.template get<0>()
      & payload.template get<1>();
  }

  friend std::size_t hash_value(const UnigramFeature<_ActionType>& s) {
    std::size_t seed = 0;
    boost::hash_combine(seed, s.payload.template get<0>());
    boost::hash_combine(seed, s.payload.template get<1>());
    return seed;
  }

  friend std::ostream& operator <<(std::ostream& os, 
      const UnigramFeature<_ActionType>& s) {
    os << "<" << s.payload.template get<0>() << ", "
      << s.payload.template get<1>()
      << ">";
    return os;
  }
};

//! Bigram score type.
template <class _ActionType>
class BigramFeature {
private:
  boost::tuples::tuple<int, int, _ActionType> payload;

public:
  BigramFeature() {}

  BigramFeature(int feat1, int feat2, const _ActionType& act)
    : payload(feat1, feat2, act) {
  }

  bool operator == (const BigramFeature<_ActionType>& a) const {
    return (
        a.payload.template get<0>() == payload.template get<0>() &&
        a.payload.template get<1>() == payload.template get<1>() &&
        a.payload.template get<2>() == payload.template get<2>());
  }

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive& ar, const unsigned version) {
    ar & payload.template get<0>()
      & payload.template get<1>()
      & payload.template get<2>();
  }

  friend std::size_t hash_value(const BigramFeature<_ActionType>& s) {
    std::size_t seed = 0;
    boost::hash_combine(seed, s.payload.template get<0>());
    boost::hash_combine(seed, s.payload.template get<1>());
    boost::hash_combine(seed, s.payload.template get<2>());
    return seed;
  }

  friend std::ostream& operator <<(std::ostream& os, 
      const BigramFeature<_ActionType>& s) {
    os << "<" << s.payload.template get<0>() << ", "
      << s.payload.template get<1>() << ", "
      << s.payload.template get<2>() << ">";
    return os;
  }
};


// Trigram
template<class _ActionType>
class TrigramFeature {
private:
  boost::tuples::tuple<int, int, int, _ActionType> payload;

public:
  TrigramFeature() {}

  TrigramFeature(int feat0, int feat1, int feat2, const _ActionType& act)
    : payload(feat0, feat1, feat2, act) {
  }

  bool operator == (const TrigramFeature<_ActionType>& a) const {
    return (
        a.payload.template get<0>() == payload.template get<0>() &&
        a.payload.template get<1>() == payload.template get<1>() &&
        a.payload.template get<2>() == payload.template get<2>() &&
        a.payload.template get<3>() == payload.template get<3>());
  }

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & payload.template get<0>()
      & payload.template get<1>()
      & payload.template get<2>()
      & payload.template get<3>();
  }

  friend std::size_t hash_value(const TrigramFeature<_ActionType>& m) {
    std::size_t seed = 0;
    boost::hash_combine(seed, m.payload.template get<0>());
    boost::hash_combine(seed, m.payload.template get<1>());
    boost::hash_combine(seed, m.payload.template get<2>());
    boost::hash_combine(seed, m.payload.template get<3>());
    return seed;
  }

  friend std::ostream& operator <<(std::ostream& os, 
      const TrigramFeature<_ActionType>& s) {
    os << "<" << s.payload.template get<0>() << ", "
      << s.payload.template get<1>() << ", "
      << s.payload.template get<2>() << ", "
      << s.payload.template get<3>() << ">";
    return os;
  }
};

}   //  end for namespace ZuoPar

#endif  //  end for __ZUOPAR_MODEL_FEATURE_H__
