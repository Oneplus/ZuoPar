#include <iostream>
#include <cstring>
#include <boost/program_options.hpp>
#include "utils/logging.h"
#include "experimental/hc_search/hstep_opt_utils.h"
#include "experimental/hc_search/arcstandard/pipe.h"
#include "frontend/common_opt_utils.h"

#define __NAME__ "HC-search depparser, arcstandard H-step"
#define __EXE__ "./bin/experimental/hc_depparser_hstep_arcstandard"

namespace hstep = ZuoPar::Experimental::HCSearchDependencyParser::HStep;
namespace po = boost::program_options;

int learn(int argc, char** argv) {
  std::string usage = "ZuoPar::" __NAME__ " Training component.\n";
  usage += "Usage: " __EXE__ " learn [options]\n\n";
  usage += "options";

  po::options_description optparser = hstep::build_learn_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, optparser), vm);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << optparser << std::endl;
    return 1;
  }
  ZuoPar::Utility::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  if (!ZuoPar::FrontEnd::check_required_options(vm, { "train" })) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  hstep::show_learn_options(vm);
  hstep::Pipe pipe(vm);
  pipe.set_signature("hc_depparser_hstep_arcstandard");
  pipe.learn();
  return 0;
}

int prepare(int argc, char** argv) {
  std::string usage = "ZuoPar::" __NAME__ " Preparing data component.\n";
  usage += "Usage: " __EXE__ " learn [options]\n\n";
  usage += "options";

  po::options_description optparser = hstep::build_prepare_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, optparser), vm);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << optparser << std::endl;
    return 1;
  }
  ZuoPar::Utility::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  if (!ZuoPar::FrontEnd::check_required_options(vm, { "input", "model" })) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  hstep::show_prepare_options(vm);
  hstep::Pipe pipe(vm);
  pipe.set_signature("hc_depparser_hstep_arcstandard");
  pipe.prepare();
  return 0;
}

int evaluate(int argc, char** argv) {
  std::string usage = "ZuoPar::" __NAME__ " Evaluation component.\n";
  usage += "Usage: " __EXE__ " learn [options]\n\n";
  usage += "options";

  po::options_description optparser = hstep::build_evaluate_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, optparser), vm);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << optparser << std::endl;
    return 1;
  }
  ZuoPar::Utility::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  if (!ZuoPar::FrontEnd::check_required_options(vm, { "input", "model" })) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  hstep::show_evaluate_options(vm);
  hstep::Pipe pipe(vm);
  pipe.evaluate();
  return 0;
}

int main(int argc, char** argv) {
  std::string usage = "ZuoPar::" __NAME__ ".\n";
  usage += "Usage: " __EXE__ " [learn|evaluate|prepare] [options]";

  if (argc == 1) {
    std::cerr << usage << std::endl;
    return 1;
  } else if (strcmp(argv[1], "learn") == 0) {
    learn(argc- 1, argv+ 1);
  } else if (strcmp(argv[1], "evaluate") == 0) {
    evaluate(argc- 1, argv+ 1);
  } else if (strcmp(argv[1], "prepare") == 0) {
    prepare(argc- 1, argv+ 1);
  } else {
    std::cerr << "Unknown mode: " << argv[1] << std::endl;
    std::cerr << usage << std::endl;
    return 1;
  }
  return 0;
}

