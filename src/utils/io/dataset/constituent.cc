#include "utils/io/dataset/constituent.h"
#include "utils/io/instance/constituent.h"

namespace ZuoPar {
namespace IO {

void read_binarized_cfg_dataset(std::istream& is,
    std::vector<BinarizedCFGTree>& dataset,
    Engine::TokenAlphabet& forms_alphabet,
    Engine::TokenAlphabet& postags_alphabet,
    Engine::TokenAlphabet& constituents_alphabet,
    size_t flag) {
  dataset.clear();
  while (!is.eof()) {
    BinarizedCFGTree tree;
    if (read_binarized_cfg_instance(is, tree, forms_alphabet,
          postags_alphabet, constituents_alphabet)) {
      dataset.push_back(tree);
    }
  }
}

} //  namespace io
} //  namespace zuopar
