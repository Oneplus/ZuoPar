#include "app/depparser/greedy_opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace DependencyParser {

po::options_description build_greedy_learn_optparser(
    const std::string& usage) {
  po::options_description opt(usage);
  opt.add_options()
    ("reference", po::value<std::string>(), "The path to the reference file.")
    ("model", po::value<std::string>(), "The path to the model file.")
    ("root", po::value<std::string>()->default_value("ROOT"), "the root tag")
    ("shuffle", po::value<int>()->default_value(0),
     "The shuffle time")
    ("help,h", "Show help information");
  return opt;
}

po::options_description build_greedy_test_optparser(
    const std::string& usage) {
  po::options_description opt(usage);
  opt.add_options()
    ("input", po::value<std::string>(), "The path to the input file.")
    ("model", po::value<std::string>(), "The path to the model file.")
    ("help", "Show help information");
  return opt;
}

bool parse_greedy_learn_option(const po::variables_map& vm, GreedyLearnOption& opts) {
  Utility::init_boost_log(false);

  if (vm.count("help")) {
    return false;
  }

  if (!vm.count("reference")) {
    _WARN << "reference file is not specified.";
    return false;
  } else {
    opts.reference_path = vm["reference"].as<std::string>();
  }

  if (!vm.count("model")) {
    _WARN << "model file is not specified.";
    return false;
  } else {
    opts.model_path = vm["model"].as<std::string>();
  }

  opts.shuffle_times = vm["shuffle"].as<int>();
  opts.root = vm["root"].as<std::string>();
  return true;
}

bool parse_greedy_test_option(const po::variables_map& vm, GreedyTestOption& opts) {
  Utility::init_boost_log(false);

  if (vm.count("help")) {
    return false;
  }

  if (!vm.count("model")) {
    _WARN << "model file is not specified.";
    return false;
  } else {
    opts.model_path = vm["model"].as<std::string>();
  }

  if (!vm.count("input")) {
    _WARN << "input file is not specified.";
    return false;
  } else {
    opts.input_path = vm["input"].as<std::string>();
  }

  return true;
}

} //  namespace dependencyparser
} //  namespace zuopar
