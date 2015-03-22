#ifndef __ZUOPAR_UTILS_IO_DATASET_BLOCK_ITERATOR_H__
#define __ZUOPAR_UTILS_IO_DATASET_BLOCK_ITERATOR_H__

#include <iostream>
#include <boost/regex.hpp>

namespace ZuoPar {
namespace IO {

class BlockIterator {
public:
  BlockIterator(std::istream& is)
    : payload((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>()),
    delimiter("\n\n"),
    itx(payload.begin(), payload.end(), delimiter, -1) {
  }

  bool end() { return itx == eos; }
  const std::string operator *() { return (*itx); }
  BlockIterator& operator ++() { ++ itx; return (*this); }

private:
  std::string payload;
  boost::regex delimiter;
  boost::sregex_token_iterator itx;
  boost::sregex_token_iterator eos;
};

}
}

#endif  //  end for __ZUOPAR_UTILS_IO_DATASET_BLOCK_ITERATOR_H__
