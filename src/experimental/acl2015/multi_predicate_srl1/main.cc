#include <iostream>
#include <cstring>  // strcmp
#include "utils/logging.h"
#include "experimental/acl2015/multi_predicate_srl1/opt.h"
#include "experimental/acl2015/multi_predicate_srl1/opt_utils.h"
#include "experimental/acl2015/multi_predicate_srl1/pipe.h"

namespace po = boost::program_options;
namespace fe = ZuoPar::FrontEnd;
namespace multi = ZuoPar::Experimental::ACL2015::MultiPredicateSRL;

int multi_learn(int argc, char** argv) {
  /*std::string usage = "Multi-threaded training component of ZuoPar::sequence labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler multi-learn [options]\n";
  usage += "OPTIONS";

  po::options_description optparser = fe::build_multi_learn_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  fe::MultiLearnOption opts;
  if (!parse_multi_learn_option_ext(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  //mono::MultiPipe pipe(opts);
  //pipe.run(); */
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
  usage += "Usage: experimental/multi_predicate_srl1 learn [options]\n";
  usage += "OPTIONS";

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
  usage += "Usage: experimental/multi_predicate_srl1 test [options]\n";
  usage += "OPTIONS";

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
MAIN("Multi-predicate Semantic Role Labeler", "experimental/multi_predicate_srl1")
