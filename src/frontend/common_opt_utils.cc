#include "frontend/common_opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace FrontEnd {

po::options_description
build_learn_optparser(const std::string& usage) {
  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h",                                "Show help information.")
    ("algorithm,a", po::value<std::string>(), "The learning algorithm.\n"
                                              " ap - average perceptron [default]\n"
                                              " pa - passive aggressive")
    ("beam,b",      po::value<int>(),         "The size for beam [default=64].")
    ("display,d",   po::value<int>(),         "The display interval [default=1000].")
    ("model,m",     po::value<std::string>(), "The path to the model.")
    ("reference,r", po::value<std::string>(), "The path to the reference file.")
    ("shuffle,s",   po::value<int>(),         "The flag for shuffling instance.\n"
                                              " 0  - not shuffle instance [default].\n"
                                              " >0 - perform s time shuffle (to avoid fake shuffling.)")
    ("update,u",    po::value<std::string>(), "Specify the early update strategy.\n"
                                              " naive - no update\n"
                                              " early - early update (Collins 04) [default]\n"
                                              " max - max violation (Huang 12)")
    ("verbose,v",                             "Logging every detail.")
  ;

  return optparser;
}

po::options_description
build_multi_learn_optparser(const std::string& usage) {
  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h",                                "Show help information.")
    ("algorithm,a", po::value<std::string>(), "The learning algorithm.\n"
                                              " ap - average perceptron [default]\n"
                                              " pa - passive aggressive")
    ("beam,b",      po::value<int>(),         "The size for beam [default=64].")
    ("batch,c",     po::value<int>(),         "The size for batch [default=16].")
    ("display,d",   po::value<int>(),         "The display interval (batch) [default=100].")
    ("model,m",     po::value<std::string>(), "The path to the model.")
    ("threads,t",   po::value<int>(),         "The number of threads [default=10].")
    ("reference,r", po::value<std::string>(), "The path to the reference file.")
    ("shuffle,s",   po::value<int>(),         "The flag for shuffling instance.\n"
                                              " 0  - not shuffle instance [default].\n"
                                              " >0 - perform s time shuffle (to avoid fake shuffling.)")
    ("update,u",    po::value<std::string>(), "Specify the early update strategy.\n"
                                              " naive - no update\n"
                                              " early - early update (Collins 04) [default]\n"
                                              " max - max violation (Huang 12)")
    ("verbose,v",                             "Logging every detail.")
  ;

  return optparser;
}

po::options_description
build_test_optparser(const std::string& usage) {
  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h",                              "Show help information.")
    ("model,m",   po::value<std::string>(), "The path to the model.")
    ("input,i",   po::value<std::string>(), "The path to the input file.")
    ("output,o",  po::value<std::string>(), "The path to the output file.")
    ("display,d", po::value<int>(),         "The display interval [default=1000].")
    ("beam,b",    po::value<int>(),         "The size for beam [default=64].")
    ("verbose,v",                           "Logging every detail.")
  ;

  return optparser;
}


bool
parse_option(const po::variables_map& vm, Option& opts) {
  namespace utils = ZuoPar::Utility;
  utils::init_boost_log(vm.count("verbose"));

  if (vm.count("help")) {
    return false;
  }

  if (!vm.count("model")) {
    _ERROR << "parse opt: model path must be specified [-m].";
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
    _INFO << "parse opt: reset beam size to " << vm["beam"].as<int>();
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
    if (vm["update"].as<std::string>() == "navie" ||
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

  opts.display_interval = 100;
  if (vm.count("display")) {
    opts.display_interval = vm["display"].as<int>();
  }

  return true;
}


bool
parse_test_option(const po::variables_map& vm, TestOption& opts) {
  if (!parse_option(vm, opts)) {
    return false;
  }

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
  return true;
}

} //  namespace frontend
} //  namespace zuopar
