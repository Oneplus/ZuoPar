#include "utils/io/instance.h"
#include <iostream>

namespace ZuoPar {
namespace IO {

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
