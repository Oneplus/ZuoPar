#include "utils/io/instance/segmentation.h"
#include <iostream>
#include <boost/regex.hpp>
#include <boost/locale.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace ZuoPar {
namespace IO {

void
read_segmentation_instance(std::istream& is,
    Segmentation& output,
    eg::TokenAlphabet& character_alphabet,
    bool incremental) {
  namespace algo = boost::algorithm;
  boost::locale::generator gen;
  //std::locale loc = gen("");
  //std::locale::global(loc);

  std::string line;
  std::getline(is, line);
  algo::trim(line);

  boost::locale::boundary::ssegment_index seq(
      boost::locale::boundary::character, line.begin(), line.end());
  boost::locale::boundary::ssegment_index::iterator p, e;
  for (p = seq.begin(), e = seq.end(); p != e; ++ p) {
    std::cout << "|" << (*p);
  }
  std::cout << "|\n";
}

} //  end for namespace IO
} //  end for namespace zuopar
