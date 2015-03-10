#ifndef __ZUOPAR_ML_PERCEPTRON_PARAM_H__
#define __ZUOPAR_ML_PERCEPTRON_PARAM_H__

#include "settings.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZuoPar {

namespace MachineLearning {

//! It's a pointwise-parameter facility that support recording the updated time
//! and averaged parameter.
struct PointwiseParameter {
  //! The non-averaged parameter.
  floatval_t w;
  //! The averaged parameter.
  floatval_t w_sum;
  //! The last time this parameter is modified.
  int w_time;

  PointwiseParameter(): w(0), w_sum(0), w_time(0) {}

  PointwiseParameter(const floatval_t& _w, const floatval_t& _w_sum, int _w_time)
    : w(_w), w_sum(_w_sum), w_time(_w_time) {}

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) { ar & w & w_sum; }

  inline floatval_t dot(bool avg) const { return (avg ? w_sum: w); }

  void add(int now, const floatval_t& scale) {
    int elapsed = now - w_time;
    floatval_t cur_val = w;
    w += scale;
    w_sum += elapsed * cur_val + scale;
    w_time = now;
  }

  void flush(int now) {
    w_sum += (now - w_time) * w;
    w_time = now;
  }

  friend std::ostream& operator <<(std::ostream& os, const PointwiseParameter& p) {
    os << "(" << p.w << ", " << p.w_sum << ", " << p.w_time << ")";
    return os;
  }
};

} //  namespace Machinelearning
} //  namespace ZuoPar

#endif  //  end for __ZGEN_ML_PERCEPTRON_PARAM_H__
