#ifndef __ZUOPAR_MODEL_SCORE_H__
#define __ZUOPAR_MODEL_SCORE_H__

#include "settings.h"
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZuoPar {

//! Unigram score type.
template <class _ActionType>
class UnigramScore {
private:
  boost::tuples::tuple<int, _ActionType> payload;

public:
  //! The empty default allocator
  UnigramScore() {}

  //! The allocator with feature and action
  UnigramScore(int feat, const _ActionType& act)
    : payload(feat, act) {
  }

  friend class boost::serialization::access;

  bool operator == (const UnigramScore<_ActionType>& a) {
    return (
        a.payload.template get<0>() == payload.template get<0>() &&
        a.payload.template get<1>().hash() == payload.template get<1>().hash());
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & payload.template get<0>()
      & payload.template get<1>();
  }

  friend std::size_t hash_value(const UnigramScore<_ActionType>& s) {
    std::size_t seed = 0;
    boost::hash_combine(seed, s.payload.template get<0>());
    boost::hash_combine(seed, s.payload.template get<1>());
    boost::hash_combine(seed, s.payload.template get<2>().hash());
    return seed;
  }
};

//! Bigram score type.
template <class _ActionType>
class BigramScore {
private:
  boost::tuples::tuple<int, int, _ActionType> payload;

public:
  BigramScore() {}

  BigramScore(int feat1, int feat2, const _ActionType& act)
    : payload(feat1, feat2, act) {
  }

  bool operator == (const BigramScore<_ActionType>& a) const {
    return (
        a.payload.template get<0>() == payload.template get<0>() &&
        a.payload.template get<1>() == payload.template get<1>() &&
        a.payload.template get<2>().hash() == payload.template get<2>().hash());
  }

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive& ar, const unsigned version) {
    ar & payload.template get<0>()
      & payload.template get<1>()
      & payload.template get<2>();
  }

  friend std::size_t hash_value(const BigramScore<_ActionType>& s) {
    std::size_t seed = 0;
    boost::hash_combine(seed, s.payload.template get<0>());
    boost::hash_combine(seed, s.payload.template get<1>());
    boost::hash_combine(seed, s.payload.template get<2>().hash());
    return seed;
  }
};


// Trigram
template<class _ActionType>
struct TrigramScore {
  TrigramScore() {}

  TrigramScore(int feat0, int feat1, int feat2, const _ActionType& act)
    : payload(feat0, feat1, feat2, act) {
  }

  bool operator == (const TrigramScore<_ActionType>& a) const {
    return (
        a.payload.template get<0>() == payload.template get<0>() &&
        a.payload.template get<1>() == payload.template get<1>() &&
        a.payload.template get<2>() == payload.template get<2>() &&
        a.payload.template get<3>().hash() == payload.template get<3>().hash());
  }

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & payload.template get<0>()
      & payload.template get<1>()
      & payload.template get<2>()
      & payload.template get<3>();
  }

  friend std::size_t hash_value(const TrigramScore<_ActionType>& m) {
    std::size_t seed = 0;
    boost::hash_combine(seed, m.payload.template get<0>());
    boost::hash_combine(seed, m.payload.template get<1>());
    boost::hash_combine(seed, m.payload.template get<2>());
    boost::hash_combine(seed, m.payload.template get<3>().hash());
    return seed;
  }

  boost::tuples::tuple<int, int, int, _ActionType> payload;
};

}   //  end for namespace ZuoPar

#endif  //  end for __ZUOPAR_MODEL_SCORE_H__
