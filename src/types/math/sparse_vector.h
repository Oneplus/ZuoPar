#ifndef __ZUOPAR_TYPES_MATH_SPARSE_VECTOR_H__
#define __ZUOPAR_TYPES_MATH_SPARSE_VECTOR_H__

#include "types/common.h"
#include <iostream>
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace ZuoPar {

//! The most basic sparse vector
typedef std::unordered_map<std::size_t, floatval_t> SparseVector;

//! The version two sparse vector, with indexed for faster compute.
typedef std::pair<std::size_t, std::size_t> SparseVector2Key;

typedef std::unordered_map<
  SparseVector2Key, floatval_t, boost::hash< SparseVector2Key >
> SparseVector2;

//! The version 3 sparse vector.
typedef std::tuple<std::size_t, std::size_t, std::size_t > SparseVector3Key;

typedef std::unordered_map<
  SparseVector3Key, floatval_t, boost::hash< SparseVector3Key >
> SparseVector3;

} // end for zuopar

#endif  //  end for __ZUOPAR_TYPES_MATH_SPARSE_VECTOR_H__
