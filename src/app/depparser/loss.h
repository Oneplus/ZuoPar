#ifndef __ZUOPAR_APP_DEPPARSER_LOSS_H__
#define __ZUOPAR_APP_DEPPARSER_LOSS_H__

namespace ZuoPar {
namespace DependencyParser {

struct Loss {
  template <class State> floatval_t operator ()(const State* predict, const State* correct) {
    floatval_t retval = 0.;
    for (auto i = 0; i < predict->ref->size(); ++ i) {
      if (predict->heads[i] != correct->heads[i])           { retval += 2.; }
      else if (predict->deprels[i] != correct->deprels[i])  { retval += 1.; }
    }
    return retval;
  }
};

}
}
#endif  //  end for __ZUOPAR_APP_DEPPARSER_LOSS_H__
