#include <iostream>
#include <cstring>  // strcmp
#include <boost/program_options.hpp>
#include "utils/logging.h"
#include "experimental/hc_search/cstep_opt.h"
#include "experimental/hc_search/cstep_opt_utils.h"
#include "experimental/hc_search/pipe.h"

#define __NAME__ "HC-search depparser, C-step"
#define __EXE__ "./bin/experimental/hc_depparser_cstep"

// namespace fe = ZuoPar::FrontEnd;
namespace cstep = ZuoPar::Experimental::HCSearchDependencyParser::CStep;
namespace po = boost::program_options;

int learn(int argc, char** argv) {
  std::string usage = "ZuoPar::" __NAME__ " Training component.\n";
  usage += "Usage: " __EXE__ " learn [options]\n\n";
  usage += "options";

  po::options_description optparser = cstep::build_learn_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  cstep::LearnOption opts;
  if (!cstep::parse_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  cstep::Pipe pipe(opts);
  pipe.learn();
  return 0;
}

int test(int argc, char** argv) {
  std::string usage = "ZuoPar::" __NAME__ " Testing component.\n";
  usage += "Usage: " __EXE__ " learn [options]\n\n";
  usage += "options";

  po::options_description optparser = cstep::build_test_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  cstep::TestOption opts;
  if (!cstep::parse_test_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  cstep::Pipe pipe(opts);
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
