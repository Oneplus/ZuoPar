#include "utils/io/instance/segmentation.h"
#include <iostream>
#include <cstdlib>
#include <boost/locale.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>

namespace ZuoPar {
namespace IO {

bool read_segmentation_instance(std::istream& is,
    Segmentation& output, bool segmented) {
  namespace algo = boost::algorithm;
  std::string line;
  std::getline(is, line);
  algo::trim(line);
  if (line.size() == 0) { return false; }

  if (segmented) {
    algo::split(output.words, line, boost::is_any_of("\t "), boost::token_compress_on);
    for (const std::string& word: output.words) {
      boost::u8_to_u32_iterator<std::string::const_iterator> p(word.begin()), e(word.end());
      for (; p != e; ++ p) {
        unsigned point = *p;
        if (point == 65279) { continue; } // BOM
        std::string ch = boost::locale::conv::utf_to_utf<char>(&point, &point + 1);
        output.chars.push_back(ch);
      }
    }
  } else {
    boost::u8_to_u32_iterator<std::string::const_iterator> p(line.begin()), e(line.end());
    for (; p != e; ++ p) {
      unsigned point = *p;
      std::string ch = boost::locale::conv::utf_to_utf<char>(&point, &point + 1);
      output.chars.push_back(ch);
    }
  }
  return true;
}

void write_segmentation_instance(std::ostream& os,
    const Segmentation& output) {
  for (int i = 0; i < output.words.size(); ++ i) {
    if (i > 0) { os << "\t"; }
    os << output.words[i];
  }
  os << std::endl;
}

} //  end for namespace IO
} //  end for namespace zuopar
