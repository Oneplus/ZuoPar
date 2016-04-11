#include "app/depparser/opt_utils.h"
#include "experimental/hc_search/cstep_opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace CStep {

namespace dp = ZuoPar::DependencyParser;
namespace po = boost::program_options;

po::options_description build_learn_optparser(const std::string& usage) {
  po::options_description optparser = dp::build_learn_optparser(usage);
  optparser.add_options()
    ("ranker", po::value<std::string>()->default_value("fine"),
     "specify the pairwise ranker:\n"
     " gold - gold against reset\n"
     " coarse - coarse one best group against reset\n"
     " fine - fine level [default]")
    ("language", po::value<std::string>()->default_value("en"),
     "The language:\n"
     " en - English\n"
     " ch - Chinese")
    ("evaluation", po::value<std::string>()->default_value("punc"),
     "The evaluation strategy:\n"
     " punc - include punctuation [default]\n"
     " conllx - conllx punctuation exclude\n"
     " chen14 - chen14 punctuation exclude, jointly determined by language.");
  return optparser;
}

po::options_description build_test_optparser(const std::string& usage) {
  po::options_description optparser = dp::build_test_optparser(usage);
  optparser.add_options()
    ("alpha", po::value<std::string>()->default_value("0"),
     "the interpolation on h-step and c-step score.");
  return optparser;
}

void show_learn_options(const boost::program_options::variables_map& vm) {
  FrontEnd::show_learn_options(vm);
  _INFO << "[RPT] ranker = " << vm["ranker"].as<std::string>();
  _INFO << "[RPT] language = " << vm["language"].as<std::string>();
  _INFO << "[RPT] evaluation = " << vm["evaluation"].as<std::string>();
}

void show_test_options(const boost::program_options::variables_map& vm) {
  FrontEnd::show_test_options(vm);
  _INFO << "[RPT] alpha = " << vm["alpha"].as<std::string>();
}

} //  namespace cstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
