#include "utils/io/dataset/csv.h"
#include "utils/io/instance/csv.h"
#include <sstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

namespace ZuoPar {
namespace IO {

void
read_csv_dataset(
    std::istream& is,
    std::vector< std::vector< std::vector<std::string> > >& dataset
    ) {
  namespace algo = boost::algorithm;
  dataset.clear();
  std::string data_context((std::istreambuf_iterator<char>(is)),
      std::istreambuf_iterator<char>());
  boost::regex INSTANCE_DELIMITER("\n\n");
  boost::sregex_token_iterator instance(data_context.begin(),
      data_context.end(), INSTANCE_DELIMITER, -1);
  boost::sregex_token_iterator eos;

  // Loop over the instances
  while (instance != eos) {
    std::istringstream iss(*instance);
    std::vector<std::vector<std::string> > csv;
    read_csv(iss, csv);
    dataset.push_back(csv);
    instance ++;
  }
}

} //  end for io
} //  end for zuopar
