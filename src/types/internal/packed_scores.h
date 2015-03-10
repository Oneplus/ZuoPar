#ifndef __ZUOPAR_TYPES_INTERNAL_PACKED_SCORE_H__
#define __ZUOPAR_TYPES_INTERNAL_PACKED_SCORE_H__

#include "types/common.h"
#include <map>
#include <boost/functional/hash.hpp>
#include <boost/serialization/map.hpp>

namespace ZuoPar {

// According to the benchmark, std::map performs better compared with
// the std::unordered_map when the size of key is small.
template <class _ActionType>
struct PackedScores: public std::map<_ActionType, floatval_t> {};

}

#endif  //  end for __ZUOPAR_TYPES_INTERNAL_PACKED_SCORE_H__
