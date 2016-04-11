#include <iostream>
#include <cstring>  // strcmp
#include <boost/program_options.hpp>
#include "utils/logging.h"
#include "experimental/hc_search/cstep_opt_utils.h"
#include "experimental/hc_search/pipe.h"
#include "frontend/common_opt_utils.h"

#define __NAME__ "HC-search depparser, C-step"
#define __EXE__ "./bin/experimental/hc_depparser_cstep"

namespace cstep = ZuoPar::Experimental::HCSearchDependencyParser::CStep;
namespace po = boost::program_options;

int learn(int argc, char** argv) {
  std::string usage = "ZuoPar::" __NAME__ " Training component.\n";
  usage += "Usage: " __EXE__ " learn [options]\n\n";
  usage += "options";

  po::options_description opt = cstep::build_learn_optparser(usage);

  if (argc == 1) {
    std::cerr << opt << std::endl;
    return 1;
  }

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, opt), vm);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << opt << std::endl;
    return 1;
  }
  ZuoPar::Utility::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) {
    std::cerr << opt << std::endl;
    return 1;
  }
  if (!ZuoPar::FrontEnd::check_required_options(vm, { "train", "script" })) {
    std::cerr << opt << std::endl;
    return 1;
  }
  cstep::show_learn_options(vm);
  cstep::Pipe pipe(vm);
  pipe.learn();
  return 0;
}

int test(int argc, char** argv) {
  std::string usage = "ZuoPar::" __NAME__ " Testing component.\n";
  usage += "Usage: " __EXE__ " learn [options]\n\n";
  usage += "options";

  po::options_description opt = cstep::build_test_optparser(usage);

  if (argc == 1) {
    std::cerr << opt << std::endl;
    return 1;
  }

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, opt), vm);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << opt << std::endl;
    return 1;
  }
  ZuoPar::Utility::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) {
    std::cerr << opt << std::endl;
    return 1;
  }
  if (!ZuoPar::FrontEnd::check_required_options(vm, { "input", "model", "script" })) {
    std::cerr << opt << std::endl;
    return 1;
  }
  cstep::show_test_options(vm);
  cstep::Pipe pipe(vm);
  pipe.test();
  return 0;
}

int main(int argc, char** argv) {
  std::string usage = "ZuoPar::" __NAME__ ".\n";
  usage += "Usage: " __EXE__ " [learn|test] [options]";

  if (argc == 1) {
    std::cerr << usage << std::endl;
    return 1;
  } else if (strcmp(argv[1], "learn") == 0) {
    learn(argc- 1, argv+ 1);
  } else if (strcmp(argv[1], "test") == 0) {
    test(argc- 1, argv+ 1);
  } else {
    std::cerr << "Unknown mode: " << argv[1] << std::endl;
    std::cerr << usage << std::endl;
    return 1;
  }

  return 0;
}
