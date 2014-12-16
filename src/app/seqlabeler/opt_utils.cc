#include "app/seqlabeler/opt_utils.h"
#include "utils/logging.h"

bool parse_option(const po::variables_map& vm, seq::Option& opts) {
  namespace utils = ZuoPar::Utility;
  utils::init_boost_log(vm.count("verbose"));

  if (vm.count("help")) {
    return false;
  }

  if (!vm.count("model")) {
    BOOST_LOG_TRIVIAL(error) << "model path must be specified [-m].";
    return false;
  } else {
    opts.model_path = vm["model"].as<std::string>();
  }

  opts.display_interval = 1000;
  if (vm.count("display")) {
    opts.display_interval = vm["display"].as<int>();
  }

  opts.conll_format = false;
  if (vm.count("conll")) {
    BOOST_LOG_TRIVIAL(info) << "use CoNLL format input and output.";
    opts.conll_format = true;
  }

  opts.beam_size = 64;
  if (vm.count("beam")) {
    BOOST_LOG_TRIVIAL(info) << "reset beam size to " << vm["beam"].as<int>();
    opts.beam_size = vm["beam"].as<int>();
  }
  return true;
}

bool parse_learn_option(const po::variables_map& vm, seq::LearnOption& opts) {
  if (!parse_option(vm, opts)) {
    return false;
  }

  if (!vm.count("reference")) {
    BOOST_LOG_TRIVIAL(error) << "reference file must be specified [-r].";
    return false;
  } else {
    opts.reference_path = vm["reference"].as<std::string>();
  }

  return true;
}

bool parse_test_option(const po::variables_map& vm, seq::TestOption& opts) {
  if (!parse_option(vm, opts)) {
    return false;
  }

  if (!vm.count("input")) {
    BOOST_LOG_TRIVIAL(error) << "input file must be specified [-i].";
    return false;
  } else {
    opts.input_path = vm["input"].as<std::string>();
  }

  opts.output_path = "";
  if (!vm.count("output")) {
    BOOST_LOG_TRIVIAL(info) << "output path not specified, use stdout instead.";
  } else {
    opts.output_path = vm["output"].as<std::string>();
  }
  return true;
}

