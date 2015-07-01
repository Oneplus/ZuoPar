#include "app/conparser/opt_utils.h"

namespace ZuoPar {
namespace ConstituentParser {

namespace fe = ZuoPar::FrontEnd;

po::options_description build_multi_learn_optparser(const std::string& usage) {
  return fe::build_multi_learn_optparser(usage);
}

po::options_description build_learn_optparser(const std::string& usage) {
  return fe::build_learn_optparser(usage);
}

po::options_description build_test_optparser(const std::string& usage) {
  return fe::build_test_optparser(usage);
}

bool parse_multi_learn_option(const po::variables_map& vm, MultiLearnOption& opts) {
  if (!fe::parse_multi_learn_option(vm, static_cast<fe::MultiLearnOption&>(opts))) {
    return false;
  }
  return true;
}

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts) {
  if (!fe::parse_learn_option(vm, static_cast<fe::LearnOption&>(opts))) {
    return false;
  }
  return true;
}

bool parse_test_option(const po::variables_map& vm, TestOption& opts) {
  if (!fe::parse_test_option(vm, static_cast<fe::TestOption&>(opts))) {
    return false;
  }
  return true;
}

} //  namespace constituentparser
} //  namespace zuopar
