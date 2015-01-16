#include "utils/io/dataset/semchunks.h"
#include "utils/io/instance/semchunks.h"
#include <sstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

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
  namespace algo = boost::algorithm;
  dataset.clear();
  std::string data_context((std::istreambuf_iterator<char>(is)),
      std::istreambuf_iterator<char>());
  boost::regex INSTANCE_DELIMITER("\n\n");
  boost::sregex_token_iterator instance(data_context.begin(),
      data_context.end(), INSTANCE_DELIMITER, -1);
  boost::sregex_token_iterator eos;

  // Loop over the instances
  while (instance != eos) {
    std::istringstream iss(*instance);
    SemanticChunks semchunks;
    read_semchunks_instance(iss, semchunks, forms_alphabet, postags_alphabet,
        senses_alphabet, chunks_alphabet, predicate_tag, incremental);
    dataset.push_back(semchunks);
    instance ++;
  }
}

void
read_semchunks_with_chunk_dataset(std::istream& is,
    std::vector<SemanticChunksWithChunks>& dataset,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& senses_alphabet,
    eg::TokenAlphabet& chunks_alphabet,
    eg::TokenAlphabet& semroles_alphabet,
    const std::string& predicate_tag,
    bool incremental) {
  namespace algo = boost::algorithm;
  dataset.clear();
  std::string data_context((std::istreambuf_iterator<char>(is)),
      std::istreambuf_iterator<char>());
  boost::regex INSTANCE_DELIMITER("\n\n");
  boost::sregex_token_iterator instance(data_context.begin(),
      data_context.end(), INSTANCE_DELIMITER, -1);
  boost::sregex_token_iterator eos;

  // Loop over the instances
  while (instance != eos) {
    std::istringstream iss(*instance);
    SemanticChunksWithChunks output;
    read_semchunks_with_chunk_instance(iss, output, forms_alphabet, postags_alphabet,
        senses_alphabet, chunks_alphabet, semroles_alphabet, predicate_tag, incremental);
    dataset.push_back(output);
    instance ++;
  }
}

} //  namespace io
} //  namespace zuopar
