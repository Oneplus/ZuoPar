#include <iostream>
#include <cstring>  // strcmp
#include "utils/logging.h"
#include "app/semchunker/mono/opt.h"
#include "app/semchunker/mono/opt_utils.h"
#include "app/semchunker/mono/pipe.h"

#define APP "mono predicate semantic labeler"
#define EXE "semchunker_mono_predicate"

namespace po = boost::program_options;
namespace fe = ZuoPar::FrontEnd;
namespace mono = ZuoPar::SemanticChunker::MonoPredicate;

int multi_learn(int argc, char** argv) {
  return 1;
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

  po::options_description optparser = fe::build_learn_optparser(usage);
  optparser.add_options()
    ("verb-class", po::value<std::string>(), "The path to the verb class.")
    ("predicate", po::value<std::string>(), "The predicate tag [default=\"V\"].")
    ("prefix", po::value<std::string>(), "The argument prefix [default=\"A\"].");

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  mono::LearnOption opts;
  if (!mono::parse_learn_option_ext(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  mono::Pipe pipe(opts);
  pipe.run();
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
  usage += "usage: " EXE "test [options]\n";
  usage += "options";

  po::options_description optparser = fe::build_test_optparser(usage);
  optparser.add_options()
    ("verb-class", po::value<std::string>(), "The path to the verb class.")
    ("predicate", po::value<std::string>(), "The predicate tag [default=\"V\"].")
    ("prefix", po::value<std::string>(), "The argument prefix [default=\"A\"].")
    ("format", po::value<std::string>(), "The output format\n"
                                         " - semchunk: the semantic chunk [default]\n"
                                         " - conll: the conll props.");

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  mono::TestOption opts;
  if (!mono::parse_test_option_ext(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  mono::Pipe pipe(opts);
  pipe.run();

  return 0;
}

#include "frontend/template/main.h"
MAIN(APP, EXE)
