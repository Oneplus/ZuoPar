#ifndef __ZUOPAR_FRONTENT_COMMON_PIPE_UTILS_H__
#define __ZUOPAR_FRONTENT_COMMON_PIPE_UTILS_H__

#include <iostream>
#include <boost/program_options.hpp>

namespace ZuoPar {
namespace FrontEnd {

std::string get_model_name(const std::string& prefix,
  const boost::program_options::variables_map& conf);

std::string get_output_name(const std::string& prefix,
  const boost::program_options::variables_map& conf);

}
}

#endif  //  end for __ZUOPAR_FRONTENT_COMMON_PIPE_H__
