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

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts) {
  if (!dp::parse_learn_option(vm, static_cast<dp::LearnOption&>(opts))) { return false; }
  opts.language = vm["language"].as<std::string>();
  if (opts.language != "en" && opts.language != "ch") {
    _WARN << "parse opt: unknown language \"" << opts.language
      << "\", reset to en";
    opts.language = "en";
  }

  opts.evaluation = vm["evaluation"].as<std::string>();
  if (opts.evaluation != "punc" && opts.evaluation != "conllx" &&
      opts.evaluation != "chen14") {
    _WARN << "parse opt: unknown evaluation strategy: \"" << opts.evaluation
      << "\", reset to punc";
    opts.evaluation = "punc";
  }

  opts.ranker = vm["ranker"].as<std::string>();
  if (opts.ranker != "gold" && opts.ranker != "coarse" && opts.ranker != "fine") {
    _WARN << "parse opt: unknown ranking method: " << opts.ranker
      << ", reset to fine";
    opts.ranker = "fine";
  }
  return true;
}

bool parse_test_option(const po::variables_map& vm, TestOption& opts) {
  if (!dp::parse_test_option(vm, static_cast<dp::TestOption&>(opts))) { return false; }
  opts.alphas = vm["alpha"].as<std::string>();
  return true;
}

} //  namespace cstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
