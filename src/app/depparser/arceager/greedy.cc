#include <iostream>
#include <cstring>
#include <boost/program_options.hpp>
#include "frontend/common_opt_utils.h"
#include "app/depparser/greedy_opt_utils.h"
#include "app/depparser/arceager/pipe.h"

#define APP "arceager greedy dependency parser"
#define EXE "arceager_depparser_greedy"

namespace po = boost::program_options;
namespace ae = ZuoPar::DependencyParser::ArcEager;
namespace dp = ZuoPar::DependencyParser;

int learn(int argc, char** argv) {
  std::string usage = "usage";
  po::options_description opt = dp::build_greedy_learn_optparser(usage);
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
  ZuoPar::FrontEnd::show_learn_options(vm);
  ae::GreedyPipe p(vm);
  p.set_signature(EXE);
  p.learn();
  return 0;
}

int test(int argc, char** argv) {
  std::string usage = "usage";
  po::options_description opt = dp::build_greedy_test_optparser(usage);
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
  ZuoPar::FrontEnd::show_test_options(vm);
  ae::GreedyPipe p(vm);
  p.test();
  return 0;
}

int main(int argc, char** argv) {
  std::string usage = "ZuoPar::" APP ".\n";
  usage += "Usage: " EXE " [learn|test] [options]";

  if (argc == 1) {
    std::cerr << usage << std::endl;
    return 1;
  } else if (strcmp(argv[1], "learn") == 0) {
    return learn(argc- 1, argv+ 1);
  } else if (strcmp(argv[1], "test") == 0) {
    return test(argc- 1, argv+ 1);
  }
  return 0;
}
