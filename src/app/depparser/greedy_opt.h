#ifndef __ZUOPAR_APP_DEPPARSER_GREEDY_OPT_H__
#define __ZUOPAR_APP_DEPPARSER_GREEDY_OPT_H__

#include <iostream>

namespace ZuoPar {
namespace DependencyParser {

struct GreedyLearnOption {
  std::string reference_path;
  std::string model_path;
  std::string root;
  int shuffle_times;
};

struct GreedyTestOption {
  std::string input_path;
  std::string output_path;
  std::string model_path;
};

} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_GREEDY_OPT_H__
