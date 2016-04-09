#include <iostream>
#include <cstring>
#include <boost/program_options.hpp>
#include "app/depparser/greedy_opt_utils.h"
#include "app/depparser/arceager/pipe.h"

#define APP "arceager greedy dependency parser"
#define EXE "arceager_depparser_greedy"

namespace po = boost::program_options;
namespace ae = ZuoPar::DependencyParser::ArcEager;
namespace dp = ZuoPar::DependencyParser;

std::string ae::GreedyPipe::signature = EXE;

int learn(int argc, char** argv) {
  std::string usage = "usage";
  po::options_description opt = dp::build_greedy_learn_optparser(usage);
  if (argc == 1) {
    std::cerr << opt << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, opt), vm);

  ae::GreedyPipe p(vm);
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
  po::store(po::parse_command_line(argc, argv, opt), vm);

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
