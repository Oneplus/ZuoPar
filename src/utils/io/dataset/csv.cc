#include "utils/io/dataset/block_iterator.h"
#include "utils/io/dataset/csv.h"
#include "utils/io/instance/csv.h"
#include <sstream>

namespace ZuoPar {
namespace IO {

void read_csv_dataset(
    std::istream& is,
    std::vector< std::vector< std::vector<std::string> > >& dataset
    ) {
  dataset.clear();
  for (BlockIterator itx(is); !itx.end(); ++ itx) {
    std::istringstream iss(*instance);
    std::vector<std::vector<std::string> > csv;
    read_csv(iss, csv);
    dataset.push_back(csv);
    instance ++;
  }
}

} //  end for io
} //  end for zuopar
