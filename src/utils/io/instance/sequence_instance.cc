#include "utils/io/instance/sequence_instance.h"
#include <iostream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace ZuoPar {
namespace IO {

bool
read_sequence_instance(std::istream& is,
    SequenceInstance& output,
    eg::TokenAlphabet& attributes_alphabet,
    eg::TokenAlphabet& tags_alphabet,
    bool incremental) {
  namespace algo = boost::algorithm;
  std::string item_context;
  while (std::getline(is, item_context)) {
    std::vector<std::string> items;
    algo::trim(item_context);
    algo::split(items, item_context, boost::is_any_of("\t "),
        boost::token_compress_on);

    SequenceItem sequence_item;
    sequence_item.tag = (incremental ?
        tags_alphabet.insert(items[0].c_str()) :
        tags_alphabet.encode(items[0].c_str())
        );
    sequence_item.attributes.clear();
    for (int i = 1; i < items.size(); ++ i) {
      attribute_t attr = (incremental ?
          attributes_alphabet.insert(items[i].c_str()):
          attributes_alphabet.encode(items[i].c_str())
          );
      if (attr > 0) {
        sequence_item.attributes.push_back(attr);
      }
    }

    output.push_back(sequence_item);
  }

  return true;
}

void
write_sequence_instance(std::ostream& os,
    const SequenceInstance& output,
    const eg::TokenAlphabet& attributes_alphabet,
    const eg::TokenAlphabet& tags_alphabet) {
  for (int i = 0; i < output.size(); ++ i) {
    os << tags_alphabet.decode(output[i].tag) << std::endl;
  }
  os << std::endl;
}

} //  end for namespace IO
} //  end for namespace zuopar
