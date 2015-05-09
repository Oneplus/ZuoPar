#include "utils/logging.h"
#include "app/depparser/nn/dataset.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

Sample::Sample() {}

Sample::Sample(const std::vector<int>& _attributes,
    const std::vector<floatval_t>& _classes)
  : attributes(_attributes), classes(_classes) {
}

Dataset::Dataset()
  : nr_attributes(0), nr_classes(0) {
}

void Dataset::add(const std::vector<int>& attributes,
    const std::vector<floatval_t>& classes) {
  samples.push_back(Sample(attributes, classes));
}

size_t Dataset::size() const { return samples.size(); }
size_t Dataset::num_attributes() const { return nr_attributes; }
size_t Dataset::num_classes() const { return nr_classes; }

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
