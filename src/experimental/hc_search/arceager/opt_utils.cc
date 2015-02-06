#include "experimental/hc_search/arceager/opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

namespace fe = ZuoPar::FrontEnd;
namespace po = boost::program_options;

po::options_description build_phase_one_learn_optparser(const std::string& usage) {
  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("algorithm,a", po::value<std::string>(), "The learning algorithm.\n"
                                              " ap - average perceptron [default]\n"
                                              " pa - passive aggressive")
    ("beam,b",      po::value<int>(),         "The size for beam [default=64].")
    ("display,d",   po::value<int>(),         "The display interval [default=1000].")
    ("reference,r", po::value<std::string>(), "The path to the reference file.")
    ("shuffle,s",   po::value<int>(),         "The flag for shuffling instance.\n"
                                              " 0  - not shuffle instance [default].\n"
                                              " >0 - perform s time shuffle (avoid fake shuffle.)")
    ("update,u",    po::value<std::string>(), "Specify the early update strategy.\n"
                                              " naive - no update\n"
                                              " early - early update (Collins 04) [default]\n"
                                              " max - max violation (Huang 12)")
    ("phase-one-model", po::value<std::string>(), "The path to the phase one model.")
    ("root",        po::value<std::string>(), "The root tag [default=ROOT].")
    ("verbose,v", "Logging every detail.")
    ;

  return optparser;
}

po::options_description build_phase_two_learn_optparser(const std::string& usage) {
  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("algorithm,a", po::value<std::string>(), "The learning algorithm.\n"
                                              " ap - average perceptron [default]\n"
                                              " pa - passive aggressive")
    ("beam,b",      po::value<int>(),         "The size for beam [default=64].")
    ("display,d",   po::value<int>(),         "The display interval [default=1000].")
    ("reference,r", po::value<std::string>(), "The path to the reference file.")
    ("shuffle,s",   po::value<int>(),         "The flag for shuffling instance.\n"
                                              " 0  - not shuffle instance [default].\n"
                                              " >0 - perform s time shuffle (avoid fake shuffle.)")
    ("phase-one-model", po::value<std::string>(), "The path to the phase one model.")
    ("phase-two-model", po::value<std::string>(), "The path to the phase two model.")
    ("root",        po::value<std::string>(), "The root tag [default=ROOT].")
    ("verbose,v", "Logging every detail.")
    ;

  return optparser;
}

po::options_description build_phase_two_prepare_optparser(const std::string& usage) {
  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h",                                "Show help information.")
    ("input,i",     po::value<std::string>(), "The path to the input file.")
    ("output,o",    po::value<std::string>(), "The path to the output file.")
    ("display,d",   po::value<int>(),         "The display interval [default=1000].")
    ("beam,b",      po::value<int>(),         "The size for beam [default=64].")
    ("root",        po::value<std::string>(), "The root tag [default=ROOT].")
    ("phase-one-model", po::value<std::string>(), "The path to the phase one model.")
    ("oracle", "Specify oracle as ybest [default=false].")
    ("verbose,v", "Logging every detail.")
    ;

  return optparser;
}

po::options_description build_test_optparser(const std::string& usage) {
  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h",                              "Show help information.")
    ("input,i",   po::value<std::string>(), "The path to the input file.")
    ("output,o",  po::value<std::string>(), "The path to the output file.")
    ("display,d", po::value<int>(),         "The display interval [default=1000].")
    ("beam,b",    po::value<int>(),         "The size for beam [default=64].")
    ("root",      po::value<std::string>(), "The root tag [default=ROOT].")
    ("phase-one-model", po::value<std::string>(), "The path to the phase one model.")
    ("phase-two-model", po::value<std::string>(), "The path to the phase two model.")
    ("rerank", "Specify to perform phase two reranking.")
    ("verbose,v", "Logging every detail.")
    ;

  return optparser;
}

po::options_description build_evaluate_optparser(const std::string& usage) {
  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h",                                "Show help information.")
    ("input,i",     po::value<std::string>(), "The path to the input file.")
    ("display,d",   po::value<int>(),         "The display interval [default=1000].")
    ("beam,b",      po::value<int>(),         "The size for beam [default=64].")
    ("root",        po::value<std::string>(), "The root tag [default=ROOT].")
    ("phase-one-model", po::value<std::string>(), "The path to the phase one model.")
    ("verbose,v", "Logging every detail.")
    ;

  return optparser;
}


bool parse_option_ext(const po::variables_map& vm, fe::Option& opts) {
  namespace utils = ZuoPar::Utility;
  utils::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) { return false; }

  opts.display_interval = 1000;
  if (vm.count("display")) { opts.display_interval = vm["display"].as<int>(); }

  opts.beam_size = 64;
  if (vm.count("beam")) { opts.beam_size = vm["beam"].as<int>(); }
  return true;
}

bool parse_input_ext(const po::variables_map& vm, fe::TestOption& opts) {
  if (!vm.count("input")) {
    _ERROR << "parse opt: input file must be specified [--input].";
    return false;
  } else {
    opts.input_path = vm["input"].as<std::string>();
  }
  return true;
}

bool parse_output_ext(const po::variables_map& vm, fe::TestOption& opts) {
  if (!vm.count("output")) {
    _INFO << "parse opt: output path not specified, use stdout instead.";
  } else {
    opts.output_path = vm["output"].as<std::string>();
  }
  return true;
}

bool parse_learn_option_ext(const po::variables_map& vm, fe::LearnOption& opts) {
  if (!vm.count("reference")) {
    _ERROR << "parse opt: reference file must be specified [--reference].";
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

bool parse_root_option(const po::variables_map& vm, RootOption& opts) {
  opts.root = "ROOT";
  if (vm.count("root")) { opts.root= vm["root"].as<std::string>(); }
  return true;
}

bool parse_phase_one_model_option(const po::variables_map& vm, PhaseOneModelOption& opts) {
  if (vm.count("phase-one-model")) {
    opts.phase_one_model_path = vm["phase-one-model"].as<std::string>();
  } else {
    _ERROR << "parse opt: phase ONE model path must be specified [--phase-one-model]";
    return false;
  }
  return true;
}

bool parse_phase_two_model_option(const po::variables_map& vm, PhaseTwoModelOption& opts) {
  if (vm.count("phase-two-model")) {
    opts.phase_two_model_path = vm["phase-two-model"].as<std::string>();
  } else {
    _ERROR << "parse opt: phase TWO model path must be specified [--phase-two-model]";
    return false;
  }
  return true;
}

bool parse_phase_one_learn_option(const po::variables_map& vm, LearnOneOption& opts) {
  if (!parse_option_ext(vm, static_cast<fe::Option&>(opts))) { return false; }
  if (!parse_learn_option_ext(vm, static_cast<fe::LearnOption&>(opts))) { return false; }
  if (!parse_phase_one_model_option(vm, static_cast<PhaseOneModelOption&>(opts))) { return false; }
  if (!parse_root_option(vm, static_cast<RootOption&>(opts))) { return false; }
  return true;
}

bool parse_phase_two_learn_option(const po::variables_map& vm, LearnTwoOption& opts) {
  if (!parse_option_ext(vm, static_cast<fe::Option&>(opts))) { return false; }
  if (!parse_learn_option_ext(vm, static_cast<fe::LearnOption&>(opts))) { return false; }
  if (!parse_phase_one_model_option(vm, static_cast<PhaseOneModelOption&>(opts))) { return false; }
  if (!parse_phase_two_model_option(vm, static_cast<PhaseTwoModelOption&>(opts))) { return false; }
  if (!parse_root_option(vm, static_cast<RootOption&>(opts))) { return false; }
  return true;
}

bool parse_phase_two_prepare_option(const po::variables_map& vm, PrepareTwoOption& opts) {
  if (!parse_option_ext(vm, static_cast<fe::Option&>(opts))) { return false; }
  if (!parse_input_ext(vm, static_cast<fe::TestOption&>(opts))) { return false; }
  if (!parse_output_ext(vm, static_cast<fe::TestOption&>(opts))) { return false; }
  if (!parse_root_option(vm, static_cast<RootOption&>(opts))) { return false; }
  if (!parse_phase_one_model_option(vm, static_cast<PhaseOneModelOption&>(opts))) { return false; }
  opts.oracle = false; if (vm.count("oracle")) { opts.oracle = true; }
  return true;
}

bool parse_test_option(const po::variables_map& vm, TestOption& opts) {
  if (!parse_option_ext(vm, static_cast<fe::Option&>(opts))) { return false; }
  if (!parse_input_ext(vm, static_cast<fe::TestOption&>(opts))) { return false; }
  if (!parse_output_ext(vm, static_cast<fe::TestOption&>(opts))) { return false; }
  opts.rerank = false; if (vm.count("rerank")) { opts.rerank = true; }
  if (!parse_root_option(vm, static_cast<RootOption&>(opts))) { return false; }
  if (!parse_phase_one_model_option(vm, static_cast<PhaseOneModelOption&>(opts))) { return false; }
  if (opts.rerank && !parse_phase_two_model_option(vm, static_cast<PhaseTwoModelOption&>(opts))) {
    return false;
  }
  return true;
}

bool parse_evaluate_option(const po::variables_map& vm, EvaluateOption& opts) {
  if (!parse_option_ext(vm, static_cast<fe::Option&>(opts))) { return false; }
  if (!parse_input_ext(vm, static_cast<fe::TestOption&>(opts))) { return false; }
  if (!parse_root_option(vm, static_cast<RootOption&>(opts))) { return false; }
  if (!parse_phase_one_model_option(vm, static_cast<PhaseOneModelOption&>(opts))) { return false; }
  return true;
}

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
