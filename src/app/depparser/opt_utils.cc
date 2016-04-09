#include "frontend/common_opt_utils.h"
#include "app/depparser/opt_utils.h"

namespace ZuoPar {
namespace DependencyParser {

po::options_description build_multi_learn_optparser(const std::string& usage) {
  po::options_description optparser = FrontEnd::build_multi_learn_optparser(usage);
  optparser.add_options()
    ("root", po::value<std::string>()->default_value("ROOT"), "The root tag.");
  return optparser;
}

po::options_description build_learn_optparser(const std::string& usage) {
  po::options_description optparser = FrontEnd::build_learn_optparser(usage);
  optparser.add_options()
    ("root", po::value<std::string>()->default_value("ROOT"), "The root tag.");
  return optparser;
}

po::options_description build_test_optparser(const std::string& usage) {
  po::options_description optparser = FrontEnd::build_test_optparser(usage);
  optparser.add_options()
    ("root", po::value<std::string>()->default_value("ROOT"), "The root tag.");
  return optparser;
}

} //  namespace dependencyparser
} //  namespace zuopar
