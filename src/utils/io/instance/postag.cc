#include "utils/io/instance/postag.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace ZuoPar {
namespace IO {

bool read_postag_instance(std::istream& is,
    Postag& output,
    eg::TokenAlphabet& postags_alphabet,
    char delimiter,
    bool incremental) {
  namespace algo = boost::algorithm;
  std::string line;
  std::getline(is, line);
  algo::trim(line);
  if (line.size() == 0) { return false; }
  std::vector<std::string> items;
  algo::split(items, line, boost::is_any_of("\t "), boost::token_compress_on);

  for (const std::string& item: items) {
    std::size_t len = item.size();
    std::size_t p = item.find_last_of(delimiter);
    BOOST_ASSERT_MSG(p != std::string::npos, "not find delimiter");

    postag_t postag = (incremental ?
        postags_alphabet.insert(item.substr(p + 1, len - p - 1)) :
        postags_alphabet.encode(item.substr(p + 1, len - p - 1)));

    output.postags.push_back(postag);
    output.forms.push_back(item.substr(0, p));
  }

  return true;
}

void write_postag_instance(std::ostream& os,
    const Postag& output,
    const eg::TokenAlphabet& postags_alphabet,
    char delimiter) {
  for (auto i = 0; i < output.size(); ++ i) {
    if (i > 0) { os << "\t"; }
    os << output.forms[i] << delimiter
      << postags_alphabet.decode(output.postags[i]);
  }
  os << std::endl;
}


} //  end for namespace IO
} //  end for namespace zuopar
