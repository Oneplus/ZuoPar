#include "utils/io/dataset/segmentation.h"
#include "utils/io/instance/segmentation.h"

namespace ZuoPar {
namespace IO {

void
read_segmentation_dataset(
    std::istream& is,
    std::vector<Segmentation>& dataset,
    eg::TokenAlphabet& characters_alphabet,
    bool incremental
    ) {
  dataset.clear();
  while (!is.eof()) {
    Segmentation segmentation;
    read_segmentation_instance(is, segmentation, characters_alphabet, incremental);
    dataset.push_back(segmentation);
  }
}

} //  end for io
} //  end for zuopar
