#include <iostream>
#include <cstring>  // strcmp
#include "utils/logging.h"
#include "app/seqlabeler/opt.h"
#include "app/seqlabeler/opt_utils.h"
#include "app/seqlabeler/pipe.h"
#include "app/seqlabeler/multi_pipe.h"

namespace po = boost::program_options;
namespace seq = ZuoPar::SequenceLabeler;
namespace fe = ZuoPar::FrontEnd;

int multi_learn(int argc, char** argv) {
  std::string usage = "Multi-threaded training component of ZuoPar::sequence labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler multi-learn [options]\n";
  usage += "OPTIONS";
  po::options_description optparser = fe::build_multi_learn_optparser(usage);
  optparser.add_options()("constrain,n", po::value<std::string>(), "The constrain file.");
  optparser.add_options()("shuffle,s", po::value<int>(), "The shuffling flags.");

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  seq::MultiLearnOption opts;
  if (!seq::parse_multi_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  seq::MultiPipe pipe(opts);
  pipe.run();
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
  std::string usage = "Training component of ZuoPar::sequence labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler learn [options]\n";
  usage += "OPTIONS";

  po::options_description optparser = fe::build_learn_optparser(usage);
  optparser.add_options()("constrain,n", po::value<std::string>(), "The constrain file.");
  optparser.add_options()("shuffle,s", po::value<int>(), "The shuffling flags.");

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  seq::LearnOption opts;
  if (!seq::parse_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  seq::Pipe pipe(opts);
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
  std::string usage = "Testing component of ZuoPar::sequence labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler test [options]\n";
  usage += "OPTIONS";

  po::options_description optparser = fe::build_test_optparser(usage);
  optparser.add_options()("constrain,n", po::value<std::string>(), "The constrain file.");

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  seq::TestOption opts;
  if (!seq::parse_test_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  seq::Pipe pipe(opts);
  pipe.run();

  return 0;
}

#include "frontend/template/main.h"
MAIN("sequence labeler", "sequence_labeler")

