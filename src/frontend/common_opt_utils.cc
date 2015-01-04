#include "frontend/common_opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace FrontEnd {

bool
parse_option(const po::variables_map& vm, Option& opts) {
  namespace utils = ZuoPar::Utility;
  utils::init_boost_log(vm.count("verbose"));

  if (vm.count("help")) {
    return false;
  }

  if (!vm.count("model")) {
    _ERROR << "model path must be specified [-m].";
    return false;
  } else {
    opts.model_path = vm["model"].as<std::string>();
  }

  opts.display_interval = 1000;
  if (vm.count("display")) {
    opts.display_interval = vm["display"].as<int>();
  }

  opts.beam_size = 64;
  if (vm.count("beam")) {
    _INFO << "reset beam size to " << vm["beam"].as<int>();
    opts.beam_size = vm["beam"].as<int>();
  }
  return true;
}

bool
parse_learn_option(const po::variables_map& vm, LearnOption& opts) {
  if (!parse_option(vm, opts)) {
    return false;
  }

  if (!vm.count("reference")) {
    _ERROR << "reference file must be specified [-r].";
    return false;
  } else {
    opts.reference_path = vm["reference"].as<std::string>();
  }

  opts.algorithm = "ap";
  if (vm.count("algorithm")) {
    if (vm["algorithm"].as<std::string>() == "pa") {
      opts.algorithm = "pa";
    }
  }

  opts.early_update = true;
  if (vm.count("no-early")) {
    opts.early_update = false;
  }
  return true;
}

bool
parse_multi_learn_option(const po::variables_map& vm, MultiLearnOption& opts) {
  if (!parse_learn_option(vm, static_cast<LearnOption&>(opts))) {
    return false;
  }

  opts.batch_size = 16;
  if (vm.count("batch")) {
    opts.batch_size = vm["batch"].as<int>();
  }

  opts.num_threads = 10;
  if (vm.count("threads")) {
    opts.num_threads = vm["threads"].as<int>();
  }
  return true;
}


bool
parse_test_option(const po::variables_map& vm, TestOption& opts) {
  if (!parse_option(vm, opts)) {
    return false;
  }

  if (!vm.count("input")) {
    _ERROR << "input file must be specified [-i].";
    return false;
  } else {
    opts.input_path = vm["input"].as<std::string>();
  }

  opts.output_path = "";
  if (!vm.count("output")) {
    _INFO << "output path not specified, use stdout instead.";
  } else {
    opts.output_path = vm["output"].as<std::string>();
  }
  return true;
}

} //  namespace frontend
} //  namespace zuopar
