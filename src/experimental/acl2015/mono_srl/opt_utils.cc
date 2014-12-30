#include "experimental/acl2015/mono_srl/opt_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoSRL{

namespace po = boost::program_options;

bool parse_test_option_ext(const po::variables_map& vm, TestOption& opts) {
  if (!parse_test_option(vm, static_cast<fe::TestOption&>(opts))) {
    return false;
  }

  opts.output_format = "semchunks";
  if (vm.count("format")) {
    opts.output_format = vm["output"].as<std::string>();
  }

  return true;
}

} //  namespace monosrl
} //  namespace acl2015
} //  namespace frontend
} //  namespace zuopar
