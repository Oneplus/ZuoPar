#include "utils/logging.h"
#include "app/depparser/nn/dataset.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

Sample::Sample() {}

Sample::Sample(const std::vector<int>& _attributes,
    const std::vector<floatval_t>& _classes)
  : attributes(_attributes),
  classes(_classes) {
}

Dataset::Dataset()
  : nr_attributes(0), nr_classes(0) {
}

void Dataset::initialize(size_t _attributes, size_t _classes) {
  if (nr_attributes!= 0 || nr_classes!= 0) {
    _WARN << "dataset: should not be initialized twice!";
    return;
  }
  if (_attributes == 0 || _classes == 0) {
    _WARN << "dataset: feature dimension/label number should not be zero";
    return;
  }

  nr_attributes= _attributes;
  nr_classes = _classes;
}

void Dataset::add(const std::vector<int>& attributes,
    const std::vector<floatval_t>& classes) {
  Sample sample(attributes, classes);
  samples.push_back(sample);
}

size_t Dataset::size() const { return samples.size(); }
size_t Dataset::num_attributes() const { return nr_attributes; }
size_t Dataset::num_classes() const { return nr_classes; }

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
