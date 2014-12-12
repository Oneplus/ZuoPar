#include "utils/io/instance/dependency.h"
#include <iostream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace ZuoPar {
namespace IO {

void
read_dependency_instance(std::istream& is,
    Dependency& output,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& deprels_alphabet,
    bool incremental) {
  namespace algo = boost::algorithm;
  std::string item_context;
  while (std::getline(is, item_context)) {
    std::vector<std::string> items;
    algo::trim(item_context);
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
    output.push_back(form, postag, atoi(items[2].c_str()), deprel);
  }
}

void
write_dependency_instance(std::ostream& os,
    const Dependency& output,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& deprels_alphabet) {
  size_t sz = output.size();
  for (size_t i = 0; i < sz; ++ i) {
    os << forms_alphabet.decode(output.forms[i])
      << "\t"
      << postags_alphabet.decode(output.postags[i])
      << "\t"
      << output.heads[i]
      << "\t"
      << deprels_alphabet.decode(output.deprels[i])
      << std::endl;
  }
  os << std::endl;
}

} //  end for namespace IO
} //  end for namespace zuopar
