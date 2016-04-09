#include <iostream>
#include <cstring>  // strcmp
#include <boost/program_options.hpp>
#include "utils/logging.h"
#include "app/depparser/opt_utils.h"
#include "app/depparser/arcstandard/pipe.h"

#define APP "arcstandard dependency parser"
#define EXE "arcstandard_depparser"

namespace dp = ZuoPar::DependencyParser;
namespace as = ZuoPar::DependencyParser::ArcStandard;
namespace po = boost::program_options;


int multi_learn(int argc, char** argv) {
  std::string usage = "Multi-threaded training component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " multi-learn [options]\n";
  usage += "options";
  po::options_description optparser = dp::build_multi_learn_optparser(usage);

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
  if (!ZuoPar::FrontEnd::check_required_options(vm, { "train", "script" })) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  ZuoPar::FrontEnd::show_multi_learn_options(vm);
  as::MultiPipe p(vm);
  p.set_signature(std::string(EXE) + std::string("_mt"));
  p.learn();
  return 0;
}

/**
 * Perform the learning process of ZuoPar::arcstandard dependency parser.
 *
 *  @param[in]  argc  The number of arguments.
 *  @param[in]  argv  The arguments.
 *  @return     int   The status of running learning.
 */
int learn(int argc, char** argv) {
  std::string usage = "Training component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " learn [options]\n";
  usage += "options";

  po::options_description optparser = dp::build_learn_optparser(usage);

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
  if (!ZuoPar::FrontEnd::check_required_options(vm, { "train", "script" })) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  ZuoPar::FrontEnd::show_learn_options(vm);
  as::Pipe p(vm);
  p.set_signature(EXE);
  p.learn();
  return 0;
}

/**
 * Perform the testing process of ZuoPar::arcstandard dependency parser.
 *
 *  @param[in]  argc  The number of arguments.
 *  @param[in]  argv  The arguments.
 *  @return     int   The status of running test.
 */
int test(int argc, char** argv) {
  std::string usage = "Testing component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " test [options]\n";
  usage += "options";

  po::options_description optparser = dp::build_test_optparser(usage);

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
  if (!ZuoPar::FrontEnd::check_required_options(vm, { "input", "model", "script" })) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  ZuoPar::FrontEnd::show_test_options(vm);
  as::Pipe p(vm);
  p.test();

  return 0;
}

#include "frontend/template/main.h"
MAIN(APP, EXE)
