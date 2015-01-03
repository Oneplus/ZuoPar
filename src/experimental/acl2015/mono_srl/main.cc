#include <iostream>
#include <cstring>  // strcmp
#include "utils/logging.h"
#include "experimental/acl2015/mono_srl/opt.h"
#include "experimental/acl2015/mono_srl/opt_utils.h"
#include "experimental/acl2015/mono_srl/pipe.h"

#define __EXPERIMENT_NAME__ "acl2015(mono SRL)"

namespace po = boost::program_options;
namespace fe = ZuoPar::FrontEnd;
namespace mono = ZuoPar::Experimental::ACL2015::MonoSRL;

int multi_learn(int argc, char** argv) {
  std::string usage = "Multi-threaded training component of ZuoPar::sequence labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler multi-learn [options]\n";
  usage += "OPTIONS";
  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("model,m", po::value<std::string>(), "The path to the model.")
    ("algorithm,a", po::value<std::string>(), "The learning algorithm.")
    ("no-early,e", "Specify the early update strategy.")
    ("batch,c", po::value<int>(), "The size for batch.")
    ("threads,t", po::value<int>(), "The number of threads.")
    ("reference,r", po::value<std::string>(), "The path to the reference file.")
    ("display,d", po::value<int>(), "The display interval.")
    ("beam,b", po::value<int>(), "The size for beam.")
    ("verbose,v", "Logging every detail.")
  ;

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  fe::MultiLearnOption opts;
  if (!parse_multi_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  //mono::MultiPipe pipe(opts);
  //pipe.run();
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

  std::string usage = "Training component of ZuoPar::acl2015(semantic role labeler).\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: acl2015 learn [options]\n";
  usage += "OPTIONS";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("model,m", po::value<std::string>(), "The path to the model.")
    ("algorithm,a", po::value<std::string>(), "The learning algorithm.")
    ("reference,r", po::value<std::string>(), "The path to the reference file.")
    ("display,d", po::value<int>(), "The display interval.")
    ("beam,b", po::value<int>(), "The size for beam.")
    ("verbose,v", "Logging every detail.")
  ;

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  fe::LearnOption opts;
  if (!parse_learn_option(vm, opts)) {
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
  std::string usage = "Testing component of ZuoPar::acl2015(semantic role labeler).\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler test [options]\n";
  usage += "OPTIONS";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("model,m", po::value<std::string>(), "The path to the model.")
    ("input,i", po::value<std::string>(), "The path to the input file.")
    ("output,o", po::value<std::string>(), "The path to the output file.")
    ("format,f", po::value<std::string>(), "The output format.")
    ("display,d", po::value<int>(), "The display interval.")
    ("beam,b", po::value<int>(), "The size for beam.")
    ("verbose,v", "Logging every detail.")
  ;

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
MAIN("mono_srl", "experimental/mono_srl_seqlabeler")
