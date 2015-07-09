#ifndef __ZUOPAR_UTILS_MISC_H__
#define __ZUOPAR_UTILS_MISC_H__

#include <iostream>
#include <vector>
#include <algorithm>

namespace ZuoPar {
namespace Utility {

static void shuffle(size_t n_items, size_t n_rounds,
    std::vector<size_t>& result) {
  result.resize(n_items);
  for (auto n = 0; n < n_items; ++ n) { result[n] = n; }
  while (n_rounds --) {
    std::random_shuffle(result.begin(), result.end());
  }
}

} //  namespace utility
} //  namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_MISC_H__
