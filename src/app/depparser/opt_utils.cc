#include "app/depparser/opt_utils.h"

namespace ZuoPar {
namespace DependencyParser {

namespace fe = ZuoPar::FrontEnd;

po::options_description build_multi_learn_optparser(const std::string& usage) {
  po::options_description optparser = fe::build_multi_learn_optparser(usage);
  optparser.add_options()("root", po::value<std::string>()->default_value("ROOT"),
      "The root tag. [default=ROOT]");
  return optparser;
}

po::options_description build_learn_optparser(const std::string& usage) {
  po::options_description optparser = fe::build_learn_optparser(usage);
  optparser.add_options()("root", po::value<std::string>()->default_value("ROOT"),
      "The root tag. [default=ROOT]");
  return optparser;
}

po::options_description build_test_optparser(const std::string& usage) {
  po::options_description optparser = fe::build_test_optparser(usage);
  optparser.add_options()("root", po::value<std::string>()->default_value("ROOT"),
      "The root tag. [default=ROOT]");
  return optparser;
}

bool parse_root_option(const po::variables_map& vm, RootOption& opts) {
  opts.root = vm["root"].as<std::string>();
  return true;
}

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts) {
  if (!fe::parse_learn_option(vm, static_cast<fe::LearnOption&>(opts))) {
    return false;
  }
  if (!parse_root_option(vm, static_cast<RootOption&>(opts))) {
    return false;
  }
  return true;
}

bool parse_multi_learn_option(const po::variables_map& vm, MultiLearnOption& opts) {
  if (!fe::parse_multi_learn_option(vm, static_cast<fe::MultiLearnOption&>(opts))) {
    return false;
  }
  if (!parse_root_option(vm, static_cast<RootOption&>(opts))) {
    return false;
  }
  return true;
}

bool parse_test_option(const po::variables_map& vm, TestOption& opts) {
  if (!fe::parse_test_option(vm, static_cast<fe::TestOption&>(opts))) {
    return false;
  }
  if (!parse_root_option(vm, static_cast<RootOption&>(opts))) {
    return false;
  }
  return true;
}

} //  namespace dependencyparser
} //  namespace zuopar
