#include <iostream>
#include <cstring>  // strcmp
#include "utils/logging.h"
#include "experimental/multi_predicate_srl/opt.h"
#include "experimental/multi_predicate_srl/opt_utils.h"
#include "experimental/multi_predicate_srl/pipe.h"

namespace po = boost::program_options;
namespace fe = ZuoPar::FrontEnd;
namespace multi = ZuoPar::Experimental::MultiPredicateSRL;

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
  std::string usage = "Training component of ZuoPar::Multi-predicate Semantic Role Labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: experimental/multi_predicate_srl learn [options]\n";
  usage += "OPTIONS";

  po::options_description optparser = multi::build_learn_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  multi::LearnOption opts;
  if (!multi::parse_learn_option_ext(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  multi::Pipe pipe(opts);
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
  std::string usage = "Testing component of ZuoPar::Multi-predicate Semantic Role Labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: experimental/multi_predicate_srl test [options]\n";
  usage += "OPTIONS";

  po::options_description optparser = multi::build_test_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  multi::TestOption opts;
  if (!multi::parse_test_option_ext(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  multi::Pipe pipe(opts);
  pipe.run();

  return 0;
}

#include "frontend/template/main.h"
MAIN("Multi-predicate Semantic Role Labeler(v2)", "experimental/multi_predicate_srl")
