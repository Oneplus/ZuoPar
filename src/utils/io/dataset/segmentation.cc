#include "utils/io/dataset/segmentation.h"
#include "utils/io/instance/segmentation.h"

namespace ZuoPar {
namespace IO {

void read_segmentation_dataset(std::istream& is,
    std::vector<Segmentation>& dataset, bool segmented) {
  dataset.clear();
  while (!is.eof()) {
    Segmentation segmentation;
    if (read_segmentation_instance(is, segmentation, segmented)) {
      dataset.push_back(segmentation);
    }
  }
}

} //  end for io
} //  end for zuopar
