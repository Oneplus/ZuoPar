#include "utils/io/dataset/block_iterator.h"
#include "utils/io/dataset/semchunks.h"
#include "utils/io/instance/semchunks.h"
#include <sstream>

namespace ZuoPar {
namespace IO {

void
read_semchunks_dataset(
    std::istream& is,
    std::vector<SemanticChunks>& dataset,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& senses_alphabet,
    eg::TokenAlphabet& chunks_alphabet,
    const std::string& predicate_tag,
    bool incremental
    ) {
  dataset.clear();
  for (BlockIterator itx = BlockIterator(is); !itx.end(); ++ itx) {
    std::istringstream iss(*itx);
    SemanticChunks semchunks;
    read_semchunks_instance(iss, semchunks, forms_alphabet, postags_alphabet,
        senses_alphabet, chunks_alphabet, predicate_tag, incremental);
    dataset.push_back(semchunks);
  }
}

} //  namespace io
} //  namespace zuopar
