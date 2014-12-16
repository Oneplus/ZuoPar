#include "utils/io/dataset/sequence_instance.h"
#include "utils/io/instance/sequence_instance.h"
#include <sstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

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
    SequenceInstance sequence_instance;
    read_sequence_instance(iss, sequence_instance,
        attributes_alphabet, tags_alphabet, incremental);
    dataset.push_back(sequence_instance);
    instance ++;
  }
}

} //  end for io
} //  end for zuopar
