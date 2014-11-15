#include "utils/io/dataset.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

namespace ZuoPar {
namespace IO {

void
read_plain_dependency_dataset(
    std::istream& is,
    std::vector<Dependency>& dataset,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& deprels_alphabet,
    bool incremental
    ) {

  namespace algo = boost::algorithm;
  dataset.clear();
  std::string data_context((std::istreambuf_iterator<char>(is)),
      std::istreambuf_iterator<char>());
  boost::regex INSTANCE_DELIMITER("\n\n");
  boost::regex ITEM_DELIMITER("\n");
  boost::sregex_token_iterator instance(data_context.begin(),
      data_context.end(), INSTANCE_DELIMITER, -1);
  boost::sregex_token_iterator eos;

  // Loop over the instances
  while (instance != eos) {
    std::string instance_context = (*instance);
    boost::sregex_token_iterator item(instance_context.begin(),
        instance_context.end(), ITEM_DELIMITER, -1);

    Dependency parse;
    while (item != eos) {
      std::string item_context = (*item);
      std::vector<std::string> items;
      algo::split(items, item_context, boost::is_any_of("\t "),
          boost::token_compress_on);

      form_t form = (incremental ?
          forms_alphabet.insert(items[0].c_str()) :
          forms_alphabet.encode(items[0].c_str())
          );
      postag_t postag = (incremental ?
          postags_alphabet.insert(items[1].c_str()) :
          postags_alphabet.encode(items[1].c_str())
          );
      deprel_t deprel = (incremental ?
          deprels_alphabet.insert(items[3].c_str()) :
          deprels_alphabet.encode(items[3].c_str())
          );
      parse.push_back(form, postag, atoi(items[2].c_str()), deprel);
      item ++;
    }
    dataset.push_back(parse);
    instance ++;
  }
}

}
}
