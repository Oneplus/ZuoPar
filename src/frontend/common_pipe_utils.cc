#include "common_pipe_utils.h"
#include "utils/misc.h"
#include <boost/lexical_cast.hpp>

namespace ZuoPar {
namespace FrontEnd {

std::string get_model_name(const std::string& prefix,
  const boost::program_options::variables_map& conf) {
  std::string ret = "";
  if (conf.count("model")) {
    ret = conf["model"].as<std::string>();
  } else {
    ret = prefix + ".";
    if (conf.count("algorithm")) {
      ret += conf["algorithm"].as<std::string>() + "_";
    }
    if (conf.count("update")) {
      ret += conf["update"].as<std::string>() + "_";
    }
    if (conf.count("beam")) {
      ret += boost::lexical_cast<std::string>(conf["beam"].as<unsigned>()) + "_";
    }
    ret += boost::lexical_cast<std::string>(Utility::get_pid()) + ".model";
  }
  return ret;
}

std::string get_output_name(const std::string& prefix,
  const boost::program_options::variables_map& conf) {
  std::string output = "";

  if (conf.count("output")) {
    output = conf["output"].as<std::string>();
  } else {
    output = prefix + ".output." + boost::lexical_cast<std::string>(Utility::get_pid());
  }

  return output;
}

} //  namespace FrontEnd
} //  namespace ZuoPar