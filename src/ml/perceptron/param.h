#ifndef __ZUOPAR_ML_PERCEPTRON_PARAM_H__
#define __ZUOPAR_ML_PERCEPTRON_PARAM_H__

#include "settings.h"

namespace ZuoPar {

namespace MachineLearning {

namespace Perceptron {

//! It's a parameter class that support recording the updated time
//! and averaged parameter.
struct Parameter {
  Parameter() : w(0), w_sum(0), w_time(0) {}

  Parameter(floatval_t _w, floatval_t _w_sum, floatval_t _w_time)
    : w(_w), w_sum(_w_sum), w_time(_w_time) {}

  floatval_t w;
  floatval_t w_sum;
  int w_time;

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & w & w_sum;
  }

  floatval_t dot(bool avg) {
    if (avg) {
      return w_sum;
    } else {
      return w;
    }
  }

  void add(int now, floatval_t scale) {
    int elapsed = now - w_time;
    floatval_t upd = scale;
    floatval_t cur_val = w;
    w = cur_val + upd;
    w_sum += elapsed * cur_val + upd;
    w_time = now;
  }

  void flush(int now) {
    w_sum += (now - w_time) * w;
    w_time = now;
  }
};

} //  end for namespace Perceptron

} //  end for namespace Machinelearning

} //  end for namespace ZuoPar

#endif  //  end for __ZGEN_ML_PERCEPTRON_PARAM_H__
