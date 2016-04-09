#include <iostream>
#include <cstring>  // strcmp
#include <boost/program_options.hpp>
#include "utils/logging.h"
#include "app/depparser/opt_utils.h"
#include "app/depparser/choi2013/pipe.h"

#define APP "dependency parser with Choi(2013)'s system"
#define EXE "depparser_choi2013"

namespace dp = ZuoPar::DependencyParser;
namespace choi = ZuoPar::DependencyParser::Choi2013;
namespace po = boost::program_options;

std::string choi::Pipe::signature = EXE;

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
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  choi::MultiPipe pipe(vm);
  pipe.learn();
  return 0;
}

/**
 * Perform the learning process of ZuoPar::arceager dependency parser.
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
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  choi::Pipe pipe(vm);
  pipe.learn();
  return 0;
}

/**
 * Perform the testing process of ZuoPar::arceager dependency parser.
 *
 *  @param[in]  argc  The number of arguments.
 *  @param[in]  argv  The arguments.
 *  @return     int   The status of running test.
 */
int test(int argc, char** argv) {
  std::string usage = "Testing component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE "test [options]\n";
  usage += "options";

  po::options_description optparser = dp::build_test_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  choi::Pipe pipe(vm);
  pipe.test();

  return 0;
}

#include "frontend/template/main.h"
MAIN(APP, EXE)
