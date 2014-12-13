#include "utils/io/instance/postag.h"
#include <iostream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace ZuoPar {
namespace IO {

bool
read_postag_instance(std::istream& is,
    Postag& output,
    eg::TokenAlphabet& forms_alphabet,
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

  for (int i = 0; i < items.size(); ++ i) {
    std::size_t len = items[i].size();
    std::size_t p = items[i].find_last_of(delimiter);
    BOOST_ASSERT_MSG(p != std::string::npos, "not find delimiter");

    form_t form = (incremental ?
        forms_alphabet.insert(items[i].substr(0, p).c_str()) :
        forms_alphabet.encode(items[i].substr(0, p).c_str())
        );

    postag_t postag = (incremental ?
        postags_alphabet.insert(items[i].substr(p+ 1, len-p-1).c_str()) :
        postags_alphabet.encode(items[i].substr(p+ 1, len-p-1).c_str())
        );

    output.push_back(form, postag);
  }

  return true;
}

void
write_postag_instance(std::ostream& os,
    const Postag& output,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    char delimiter) {
  size_t sz = output.size();
  for (size_t i = 0; i < sz; ++ i) {
    os << forms_alphabet.decode(output.forms[i])
      << delimiter
      << postags_alphabet.decode(output.postags[i]);
    if (i > 0) {
      os << "\t";
    }
  }
  os << std::endl;
}

bool
read_postag_with_cache_instance(std::istream& is,
    PostagWithLiteralCache& output,
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

  for (int i = 0; i < items.size(); ++ i) {
    std::size_t len = items[i].size();
    std::size_t p = items[i].find_last_of(delimiter);
    BOOST_ASSERT_MSG(p != std::string::npos, "not find delimiter");

    postag_t postag = (incremental ?
        postags_alphabet.insert(items[i].substr(p + 1, len - p - 1).c_str()) :
        postags_alphabet.encode(items[i].substr(p + 1, len - p - 1).c_str())
        );

    output.postags.push_back(postag);
    output.cache.push_back(items[i].substr(0, p));
  }

  return true;
}

void
write_postag_with_cache_instance(std::ostream& os,
    const PostagWithLiteralCache& output,
    const eg::TokenAlphabet& postags_alphabet,
    char delimiter) {
  size_t sz = output.size();
  for (size_t i = 0; i < sz; ++ i) {
    if (i > 0) { os << "\t"; }
    os << output.cache[i] << delimiter
      << postags_alphabet.decode(output.postags[i]);
  }
  os << std::endl;
}


} //  end for namespace IO
} //  end for namespace zuopar
