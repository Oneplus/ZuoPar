#ifndef __ZUOPAR_TYPES_INTERNAL_PACKED_SCORE_H__
#define __ZUOPAR_TYPES_INTERNAL_PACKED_SCORE_H__

#include <boost/functional/hash.hpp>
#include <unordered_map>
#include "utils/serialization/unordered_map.h"

namespace ZuoPar {

template <class _ActionType>
struct PackedScores: public std::unordered_map<
  _ActionType,
  floatval_t,
  boost::hash<_ActionType> > {
};

}

#endif  //  end for __ZUOPAR_TYPES_INTERNAL_PACKED_SCORE_H__
