#include "app/depparser/opt_utils.h"
#include "experimental/hc_search/hstep_opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace HStep {

namespace dp = ZuoPar::DependencyParser;
namespace po = boost::program_options;

po::options_description build_learn_optparser(const std::string& usage) {
  po::options_description optparser = dp::build_learn_optparser(usage);
  optparser.add_options()
    ("neg-sample", po::value<std::string>()->default_value("best"),
     "The select negative sample strategy:\n"
     " baseline - update both fallout and final.\n"
     " best - update with the best [default].\n"
     " random - update with the random.\n"
     " worst - update with the worst.");

  return optparser;
}


po::options_description build_prepare_optparser(const std::string& usage) {
  po::options_description optparser = dp::build_test_optparser(usage);
  optparser.add_options()
    ("language", po::value<std::string>()->default_value("en"),
     "The language for evaluation:\n"
     " en - English\n"
     " ch - Chinese");

  return optparser;
}

po::options_description build_evaluate_optparser(const std::string& usage) {
  po::options_description optparser = dp::build_test_optparser(usage);
  optparser.add_options()
    ("language", po::value<std::string>()->default_value("en"),
     "The language for evaluation:\n"
     " en - English\n"
     " ch - Chinese");

  return optparser;
}

bool _parse_language_option(const po::variables_map& vm, LanguageOption& opts) {
  opts.language = vm["language"].as<std::string>();
  if (opts.language != "en" && opts.language != "ch") {
    _WARN << "parse opt: unknown language \"" << opts.language
      << "\", reset to en";
    opts.language = "en";
  }
  return true;
}

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts) {
  if (!dp::parse_learn_option(vm, static_cast<dp::LearnOption&>(opts))) {
    return false;
  }

  opts.neg_sample = vm["neg-sample"].as<std::string>();
  if (opts.neg_sample != "baseline" && opts.neg_sample != "best" &&
      opts.neg_sample != "random" && opts.neg_sample != "worst") {
    _WARN << "parse opt: unknown neg-sample strategy: " << opts.neg_sample
      << ", reset to best";
    opts.neg_sample = "best";
  }
  return true;
}

bool parse_prepare_option(const po::variables_map& vm, PrepareOption& opts) {
  if (!dp::parse_test_option(vm, static_cast<dp::TestOption&>(opts))) {
    return false;
  }

  if (!_parse_language_option(vm, static_cast<LanguageOption&>(opts))) {
    return false;
  }

  return true;
}

bool parse_evaluate_option(const po::variables_map& vm, EvaluateOption& opts) {
  if (!dp::parse_test_option(vm, static_cast<dp::TestOption&>(opts))) {
    return false;
  }

  if (!_parse_language_option(vm, static_cast<LanguageOption&>(opts))) {
    return false;
  }

  return true;
}

} //  namespace hstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
