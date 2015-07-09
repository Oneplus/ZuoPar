#include "app/seqlabeler/opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace SequenceLabeler {

namespace fe = ZuoPar::FrontEnd;

bool parse_constrain_option(const po::variables_map& vm, ConstrainOption& opts) {
  opts.constrain_path = "";
  if (vm.count("constrain")) {
    opts.constrain_path = vm["constrain"].as<std::string>();
  }
  return true;
}

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts) {
  if (!fe::parse_learn_option(vm, static_cast<fe::LearnOption&>(opts))) {
    return false;
  }

  return parse_constrain_option(vm, static_cast<ConstrainOption&>(opts));
}

bool parse_multi_learn_option(const po::variables_map& vm, MultiLearnOption& opts) {
  if (!parse_learn_option(vm, static_cast<LearnOption&>(opts))) {
    return false;
  }
  opts.batch_size = vm["batch"].as<int>();
  opts.num_threads = vm["threads"].as<int>();
  return true;
}

bool parse_test_option(const po::variables_map& vm, TestOption& opts) {
  if (!fe::parse_test_option(vm, static_cast<fe::TestOption&>(opts))) {
    return false;
  }
  return parse_constrain_option(vm, static_cast<ConstrainOption&>(opts));
}

} //  namespace sequencelabeler
} //  namespace zuopar
