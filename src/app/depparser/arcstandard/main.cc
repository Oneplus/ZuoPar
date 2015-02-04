#include <iostream>
#include <cstring>  // strcmp
#include <boost/program_options.hpp>
#include "utils/logging.h"
#include "app/depparser/arcstandard/opt.h"
#include "app/depparser/arcstandard/opt_utils.h"
#include "app/depparser/arcstandard/pipe.h"

namespace dp = ZuoPar::DependencyParser;
namespace as = ZuoPar::DependencyParser::ArcStandard;
namespace po = boost::program_options;

int multi_learn(int argc, char** argv) {
  std::string usage = "Multi-threaded training component of ZuoPar::sequence labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler multi-learn [options]\n";
  usage += "OPTIONS";
  po::options_description optparser = dp::build_multi_learn_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  dp::MultiLearnOption opts;
  if (!dp::parse_multi_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  as::MultiPipe pipe(opts);
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
  std::string usage = "Training component of ZuoPar::arcstandard dependency parser.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: arcstandard_depparser learn [options]\n";
  usage += "OPTIONS";

  po::options_description optparser = dp::build_learn_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  dp::LearnOption opts;
  if (!dp::parse_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  as::Pipe pipe(opts);
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

  std::string usage = "Testing component of ZuoPar::arcstandard dependency parser.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: arcstandard_depparser test [options]\n";
  usage += "OPTIONS";

  po::options_description optparser = dp::build_test_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  dp::TestOption opts;
  if (!dp::parse_test_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  as::Pipe pipe(opts);
  pipe.run();

  return 0;
}

#include "frontend/template/main.h"
MAIN("arcstandard dependency parser.", "arcstandard_depparser")

