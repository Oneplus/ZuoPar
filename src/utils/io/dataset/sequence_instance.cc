#include "utils/io/dataset/block_iterator.h"
#include "utils/io/dataset/sequence_instance.h"
#include "utils/io/instance/sequence_instance.h"
#include <sstream>

namespace ZuoPar {
namespace IO {

void
read_sequence_instance_dataset(
    std::istream& is,
    std::vector<SequenceInstance>& dataset,
    eg::TokenAlphabet& attributes_alphabet,
    eg::TokenAlphabet& tags_alphabet,
    bool incremental
    ) {

  dataset.clear();
  for (BlockIterator itx = BlockIterator(is); !itx.end(); ++ itx) {
    std::istringstream iss(*itx);
    SequenceInstance sequence_instance;
    read_sequence_instance(iss, sequence_instance,
        attributes_alphabet, tags_alphabet, incremental);
    dataset.push_back(sequence_instance);
  }
}

} //  end for io
} //  end for zuopar
