#include "app/depparser/greedy_opt_utils.h"
#include "frontend/common_opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace DependencyParser {

po::options_description build_greedy_learn_optparser(
    const std::string& usage) {
  po::options_description opt = FrontEnd::build_learn_optparser(usage);
  opt.add_options()("root", po::value<std::string>()->default_value("ROOT"), "the root tag");
  return opt;
}

po::options_description build_greedy_test_optparser(
    const std::string& usage) {
  po::options_description opt = FrontEnd::build_test_optparser(usage);
  return opt;
}

} //  namespace dependencyparser
} //  namespace zuopar
