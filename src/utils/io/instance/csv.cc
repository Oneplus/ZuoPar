#include "utils/io/instance/csv.h"
#include "utils/logging.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace ZuoPar {
namespace IO {

void read_csv(std::istream& is,
    std::vector< std::vector<std::string> >& output) {
  namespace algo = boost::algorithm;
  std::string line;
  while (std::getline(is, line)) {
    algo::trim(line);
    if (line.size() == 0) { return; }
    std::vector<std::string> items;
    algo::split(items, line, boost::is_any_of("\t "), boost::token_compress_on);
    output.push_back(items);
  }
}

void write_csv(std::ostream& os,
    const std::vector< std::vector<std::string> >& output,
    const std::string& delimiter) {
  if (output.size() == 0) {
    _WARN << "io: write_csv, the matrix has zero lines";
    return;
  }

  int N = output[0].size();
  for (const std::vector<std::string>& row: output) {
    for (int j = 0; j < N; ++ j) {
      os << row[j];
      if (j +1 < N) { os << delimiter; }
    }
    os << std::endl;
  }
  os << std::endl;
}

} //  namespace io
} //  namespace zuopar
