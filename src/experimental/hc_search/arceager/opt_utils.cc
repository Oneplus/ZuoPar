#include "experimental/hc_search/arceager/opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

namespace fe = ZuoPar::FrontEnd;
namespace po = boost::program_options;

bool
parse_option_ext(const po::variables_map& vm, fe::Option& opts) {
  namespace utils = ZuoPar::Utility;
  utils::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) { return false; }

  opts.display_interval = 1000;
  if (vm.count("display")) { opts.display_interval = vm["display"].as<int>(); }

  opts.beam_size = 64;
  if (vm.count("beam")) { opts.beam_size = vm["beam"].as<int>(); }
  return true;
}

bool
parse_learn_option_ext(const po::variables_map& vm, fe::LearnOption& opts) {
  if (!vm.count("reference")) {
    _ERROR << "parse opt: reference file must be specified [-r].";
    return false;
  } else {
    opts.reference_path = vm["reference"].as<std::string>();
  }

  opts.algorithm = "ap";
  if (vm.count("algorithm")) {
    if (vm["algorithm"].as<std::string>() == "pa" ||
        vm["algorithm"].as<std::string>() == "ap") {
      opts.algorithm = vm["algorithm"].as<std::string>();
    } else {
      _WARN << "parse opt: unknown algorithm \""
        << vm["algorithm"].as<std::string>() << "\".";
    }
  }

  opts.update_strategy = "early";
  if (vm.count("update")) {
    if (vm["update"].as<std::string>() == "naive" ||
        vm["update"].as<std::string>() == "early" ||
        vm["update"].as<std::string>() == "max") {
      opts.update_strategy = vm["update"].as<std::string>();
    } else {
      _WARN << "parse opt: unknown update strategy \""
        << vm["update"].as<std::string>() << "\".";
    }
  }

  opts.shuffle_times = 0;
  if (vm.count("shuffle")) {
    opts.shuffle_times = vm["shuffle"].as<int>();
  }
  return true;
}

bool
parse_phase_one_learn_option(const po::variables_map& vm, LearnOneOption& opts) {
  if (!parse_option_ext(vm, static_cast<fe::Option&>(opts))) { return false; }
  if (!parse_learn_option_ext(vm, static_cast<fe::LearnOption&>(opts))) { return false; }
  if (vm.count("phase-one-model")) {
    opts.phase_one_model_path = vm["phase-one-model"].as<std::string>();
  } else {
    _ERROR << "parse opt: phase ONE model path must be specified [--phase-one-model]";
    return false;
  }
  return true;
}

bool
parse_phase_two_learn_option(const po::variables_map& vm, LearnTwoOption& opts) {
  if (!parse_option_ext(vm, static_cast<fe::Option&>(opts))) { return false; }
  if (!parse_learn_option_ext(vm, static_cast<fe::LearnOption&>(opts))) { return false; }

  opts.update_strategy = "naive";
  if (vm.count("phase-one-model")) {
    opts.phase_one_model_path = vm["phase-one-model"].as<std::string>();
  } else {
    _ERROR << "parse opt: phase ONE model path must be specified [--phase-one-model]";
    return false;
  }
  if (vm.count("phase-two-model")) {
    opts.phase_two_model_path = vm["phase-two-model"].as<std::string>();
  } else {
    _ERROR << "parse opt: phase TWO model path must be specified [--phase-two-model]";
    return false;
  }
  return true;
}

bool
parse_test_option(const po::variables_map& vm, TestOption& opts) {
  if (!parse_option_ext(vm, static_cast<fe::Option&>(opts))) { return false; }
  if (!vm.count("input")) {
    _ERROR << "parse opt: input file must be specified [-i].";
    return false;
  } else {
    opts.input_path = vm["input"].as<std::string>();
  }

  opts.output_path = "";
  if (!vm.count("output")) {
    _INFO << "parse opt: output path not specified, use stdout instead.";
  } else {
    opts.output_path = vm["output"].as<std::string>();
  }

  if (!vm.count("phase-one-model")) {
    _ERROR << "parse opt: phase one model should be specified [--phase-one-model]";
  } else {
    opts.phase_one_model_path = vm["phase-one-model"].as<std::string>();
  }

  if (!vm.count("phase-two-model")) {
    _ERROR << "parse opt: phase two model should be specified [--phase-two-model]";
  } else {
    opts.phase_two_model_path = vm["phase-two-model"].as<std::string>();
  }
  return true;
}

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
