#include "utils/io/dataset/block_iterator.h"
#include "utils/io/dataset/dependency.h"
#include "utils/io/instance/dependency.h"
#include <sstream>

namespace ZuoPar {
namespace IO {

void read_dependency_dataset(std::istream& is,
    std::vector<Dependency>& dataset,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& deprels_alphabet,
    size_t flag) {
  dataset.clear();
  for (BlockIterator itx = BlockIterator(is); !itx.end(); ++ itx) {
    std::istringstream iss(*itx);
    Dependency parse;
    read_dependency_instance(iss, parse, forms_alphabet,
        postags_alphabet, deprels_alphabet, flag);
    dataset.push_back(parse);
  }
}

void read_conllx_dependency_dataset(std::istream& is,
    std::vector<CoNLLXDependency>& dataset,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& lemmas_alphabet,
    eg::TokenAlphabet& cpostags_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& feat_alphabet,
    eg::TokenAlphabet& deprels_alphabet,
    size_t flag) {
  dataset.clear();
  for (BlockIterator itx = BlockIterator(is); !itx.end(); ++ itx) {
    std::istringstream iss(*itx);
    CoNLLXDependency parse;
    read_conllx_dependency_instance(iss, parse, forms_alphabet,
        lemmas_alphabet, cpostags_alphabet, postags_alphabet,
        feat_alphabet, deprels_alphabet, flag);
    dataset.push_back(parse);
  }
}

void read_raw_dependency_dataset(std::istream& is,
    std::vector<RawDependency>& dataset) {
  dataset.clear();
  for (BlockIterator itx = BlockIterator(is); !itx.end(); ++ itx) {
    std::istringstream iss(*itx);
    RawDependency parse;
    read_raw_dependency_instance(iss, parse);
    dataset.push_back(parse);
  }
}

void read_raw_conllx_dependency_dataset(std::istream& is,
    std::vector<RawCoNLLXDependency>& dataset) {
  dataset.clear();
  for (BlockIterator itx = BlockIterator(is); !itx.end(); ++ itx) {
    std::istringstream iss(*itx);
    RawCoNLLXDependency parse;
    read_raw_conllx_dependency_instance(iss, parse);
    dataset.push_back(parse);
  }
}


} //  end for io
} //  end for zuopar
