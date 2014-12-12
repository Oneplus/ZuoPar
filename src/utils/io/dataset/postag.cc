#include "utils/io/dataset/postag.h"
#include "utils/io/instance/postag.h"

namespace ZuoPar {
namespace IO {

void
read_postag_dataset(
    std::istream& is,
    std::vector<Postag>& dataset,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    char delimiter,
    bool incremental
    ) {
  dataset.clear();
  while (!is.eof()) {
    Postag postag;
    read_postag_instance(is, postag, forms_alphabet, postags_alphabet,
        delimiter, incremental);
    dataset.push_back(postag);
  }
}

void
read_postag_with_cache_dataset(
    std::istream& is,
    std::vector<PostagWithLiteralCache>& dataset,
    eg::TokenAlphabet& postags_alphabet,
    char delimiter,
    bool incremental
    ) {
  dataset.clear();
  while (!is.eof()) {
    PostagWithLiteralCache postag;
    read_postag_with_cache_instance(is, postag, postags_alphabet, delimiter, incremental);
    dataset.push_back(postag);
  }
}

}
}
