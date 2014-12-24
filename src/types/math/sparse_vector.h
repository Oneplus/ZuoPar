#ifndef __ZUOPAR_TYPES_MATH_SPARSE_VECTOR_H__
#define __ZUOPAR_TYPES_MATH_SPARSE_VECTOR_H__

#include "types/common.h"
#include <iostream>
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace ZuoPar {

//! The most basic sparse vector
typedef std::unordered_map<int, floatval_t> SparseVector;

//! The version two sparse vector, with indexed for faster compute.
typedef std::unordered_map<std::pair<int, std::size_t>, floatval_t,
        boost::hash<std::pair<int, std::size_t> > > SparseVector2;

} // end for zuopar

#endif  //  end for __ZUOPAR_TYPES_MATH_SPARSE_VECTOR_H__
