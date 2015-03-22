#include "utils/io/dataset/block_iterator.h"
#include "utils/io/dataset/chunks.h"
#include "utils/io/instance/chunks.h"
#include "utils/logging.h"
#include <sstream>

namespace ZuoPar {
namespace IO {

void
read_chunks_with_semantic_roles_dataset(std::istream& is,
    std::vector<ChunksWithSemanticRoles>& dataset,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& senses_alphabet,
    eg::TokenAlphabet& chunks_alphabet,
    eg::TokenAlphabet& semroles_alphabet,
    const std::string& predicate_tag,
    bool incremental) {

  dataset.clear();
  for (BlockIterator itx = BlockIterator(is); !itx.end(); ++ itx) {
    std::istringstream iss(*itx);
    ChunksWithSemanticRoles output;
    read_chunks_with_semantic_roles_instance(iss, output, forms_alphabet,
        postags_alphabet, senses_alphabet, chunks_alphabet, semroles_alphabet,
        predicate_tag, incremental);
    dataset.push_back(output);
  }
}

} //  namespace io
} //  namespace zuopar
