#include "frontend/common_opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace FrontEnd {

po::options_description build_learn_optparser(const std::string& usage) {
  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("verbose,v", "Logging every detail.")
    ("train,r", po::value<std::string>(), "The path to the reference file.")
    ("devel,d", po::value<std::string>(), "The path to the development data.")
    ("model,m", po::value<std::string>(), "The path to the model.")
    ("beam", po::value<unsigned>()->default_value(64), "The size for beam.")
    ("algorithm", po::value<std::string>()->default_value("ap"),
    "The learning algorithm.\n"
    " ap - average perceptron\n"
    " pa - passive aggressive")
    ("update", po::value<std::string>()->default_value("early"),
    "Specify the update strategy.\n"
    " naive - no update\n"
    " early - early update (Collins 04)"
    " max - max violation (Huang 12)")
    ("report_stops", po::value<unsigned>()->default_value(1000), "The report frequency.")
    ("evaluate_stops", po::value<unsigned>()->default_value(5000), "The evaluation frequency.")
    ("script", po::value<std::string>(), "The path to the evaluation script.")
    ("output", po::value<std::string>(), "The path to the output file (temporal).")
    ("maxiter", po::value<unsigned>()->default_value(10), "The maximum number of iteration.")
    ;

  return optparser;
}

po::options_description build_multi_learn_optparser(const std::string& usage) {
  po::options_description optparser = build_learn_optparser(usage);
  optparser.add_options()
    ("batch", po::value<int>()->default_value(16), "The size for batch.")
    ("threads", po::value<int>()->default_value(10), "The number of threads.")
    ;

  return optparser;
}

po::options_description build_test_optparser(const std::string& usage) {
  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("verbose,v", "Logging every detail.")
    ("model,m", po::value<std::string>(), "The path to the model.")
    ("input,i", po::value<std::string>(), "The path to the input file.")
    ("output", po::value<std::string>(), "The path to the output file.")
    ("beam", po::value<unsigned>()->default_value(64), "The size for beam.")
    ("script", po::value<std::string>(), "The path to the evaluation script.")
    ;

  return optparser;
}

void show_learn_options(const boost::program_options::variables_map& vm) {
  _INFO << "[OPT] #beam: " << vm["beam"].as<unsigned>();
  _INFO << "[OPT] algorithm: " << vm["algorithm"].as<std::string>();
  _INFO << "[OPT] update: " << vm["update"].as<std::string>();
  _INFO << "[OPT] report every: " << vm["report_stops"].as<unsigned>() << " stops";
  _INFO << "[OPT] evaluate every: " << vm["evaluate_stops"].as<unsigned>() << " stops";
  _INFO << "[OPT] #iteration: " << vm["maxiter"].as<unsigned>();
}

void show_multi_learn_options(const boost::program_options::variables_map& vm) {
  show_learn_options(vm);
  _INFO << "[OPT] #batch: " << vm["batch"].as<unsigned>();
  _INFO << "[OPT] #threads: " << vm["threads"].as<unsigned>();
}

void show_test_options(const boost::program_options::variables_map& vm) {
  _INFO << "[OPT] #beam: " << vm["beam"].as<unsigned>();
}

bool check_required_options(const boost::program_options::variables_map& vm,
  const std::vector<std::string>& required) {
  for (const std::string& option : required) {
    if (!vm.count(option)) {
      _ERROR << "option: " << option << " is required, but not set.";
      return false;
    }
  }
  return true;
}

bool parse_option(const po::variables_map& vm, Option& opts) {
  Utility::init_boost_log(vm.count("verbose"));

  if (vm.count("help")) { return false; }

  if (!vm.count("model")) {
    _ERROR << "parse opt: model path must be specified [-m].";
    return false;
  } else {
    opts.model_path = vm["model"].as<std::string>();
  }

  opts.display_interval = vm["display"].as<int>();
  opts.beam_size = vm["beam"].as<int>();
  return true;
}

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts) {
  if (!parse_option(vm, opts)) { return false; }

  if (!vm.count("reference")) {
    _ERROR << "parse opt: reference file must be specified [-r].";
    return false;
  } else {
    opts.reference_path = vm["reference"].as<std::string>();
  }

  opts.algorithm = vm["algorithm"].as<std::string>();
  if (opts.algorithm != "pa" && opts.algorithm != "ap") {
    _WARN << "parse opt: unknown algorithm \"" << opts.algorithm << "\", reset to ap.";
    opts.algorithm = "ap";
  }

  opts.update_strategy = vm["update"].as<std::string>();
  if (opts.update_strategy != "naive" &&
      opts.update_strategy != "early" &&
      opts.update_strategy != "max") {
    _WARN << "parse opt: unknown update strategy \"" << opts.update_strategy
      << "\", reset to early.";
  }

  opts.shuffle_times = vm["shuffle"].as<int>();
  if (opts.shuffle_times < 0) {
    _WARN << "parse opt: illegal shuffle flag :" << opts.shuffle_times
        << " not shuffling.";
    opts.shuffle_times = 0;
  }

  return true;
}

bool parse_multi_learn_option(const po::variables_map& vm, MultiLearnOption& opts) {
  if (!parse_learn_option(vm, static_cast<LearnOption&>(opts))) {
    return false;
  }

  opts.batch_size = vm["batch"].as<int>();
  opts.num_threads = vm["threads"].as<int>();
  return true;
}


bool parse_test_option(const po::variables_map& vm, TestOption& opts) {
  if (!parse_option(vm, opts)) { return false; }

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
