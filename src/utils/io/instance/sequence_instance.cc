#include "utils/io/instance/sequence_instance.h"
#include "utils/io/instance/csv.h"
#include <iostream>

namespace ZuoPar {
namespace IO {

bool read_sequence_instance(std::istream& is,
    SequenceInstance& output,
    eg::TokenAlphabet& attributes_alphabet,
    eg::TokenAlphabet& tags_alphabet,
    bool incremental) {
  std::vector< std::vector<std::string> > mat;
  read_csv(is, mat);
  for (const std::vector<std::string>& items: mat) {
    SequenceItem sequence_item;
    sequence_item.tag = (incremental ?
        tags_alphabet.insert(items[0]) :
        tags_alphabet.encode(items[0]));
    sequence_item.attributes.clear();
    for (const std::string& token: items) {
      attribute_t attr = (incremental ?
          attributes_alphabet.insert(token):
          attributes_alphabet.encode(token));
      if (attr > 0) { sequence_item.attributes.push_back(attr); }
    }

    output.push_back(sequence_item);
  }

  return true;
}

void write_sequence_instance(std::ostream& os,
    const SequenceInstance& output,
    const eg::TokenAlphabet& attributes_alphabet,
    const eg::TokenAlphabet& tags_alphabet) {
  for (const SequenceItem& o: output) {
    os << tags_alphabet.decode(o.tag) << std::endl;
  }
  os << std::endl;
}

} //  end for namespace IO
} //  end for namespace zuopar
