#ifndef __ZUOPAR_UTILS_MISC_H__
#define __ZUOPAR_UTILS_MISC_H__

#include <iostream>
#include <vector>
#include <algorithm>
#ifdef _MSC_VER
#include <Windows.h>
#endif

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

static double execute_script(const std::string& script, const std::string& output) {
#ifndef _MSC_VER
  std::string cmd = script + " " + output;
  _TRACE << "Running: " << cmd << std::endl;
  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe) {
    return 0.;
  }
  char buffer[128];
  std::string result = "";
  while (!feof(pipe)) {
    if (fgets(buffer, 128, pipe) != NULL) { result += buffer; }
  }
  pclose(pipe);

  std::stringstream S(result);
  std::string token;
  while (S >> token) {
    boost::algorithm::trim(token);
    return boost::lexical_cast<double>(token);
  }
#else
  return 1.;
#endif
  return 0.;
}

static unsigned get_pid() {
#ifndef _MSC_VER
  return getpid();
#endif
  return GetCurrentProcessId();
}

} //  namespace utility
} //  namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_MISC_H__
