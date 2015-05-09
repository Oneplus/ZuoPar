#ifndef __ZUOPAR_APP_DEPPARSER_NN_DATASET_H__
#define __ZUOPAR_APP_DEPPARSER_NN_DATASET_H__

#include <vector>
#include <cstdlib>
#include "types/common.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

struct Sample {
  std::vector<int> attributes;      //! sparse vector of attributes
  std::vector<floatval_t> classes;  //! dense vector of classes

  Sample();
  Sample(const std::vector<int>& attributes,
      const std::vector<floatval_t>& classes);
};

struct Dataset {
  size_t nr_attributes; //  nr_forms + nr_postags + nr_deprels
  size_t nr_classes;    //  nr_deprels * 2 - 1
  std::vector<Sample> samples;

  Dataset();
  void add(const std::vector<int>& attributes, const std::vector<floatval_t>& classes);
  void shuffle();
  size_t size() const;
  size_t num_attributes() const;
  size_t num_classes() const;
};

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_NN_DATASET_H__
