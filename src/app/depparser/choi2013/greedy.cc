#include <iostream>
#include <cstring>
#include <boost/program_options.hpp>
#include "app/depparser/greedy_opt_utils.h"
#include "app/depparser/choi2013/pipe.h"

#define APP "greedy dependency parser with Choi(2013)'s system"
#define EXE "depparser_choi2013_greedy"

namespace po = boost::program_options ;
namespace choi = ZuoPar::DependencyParser::Choi2013 ;
namespace dp = ZuoPar::DependencyParser ;


int learn(int argc , char ** argv) {
  std::string usage = "usage" ;
  po::options_description opt = dp::build_greedy_learn_optparser(usage) ;

  if(argc == 1) {
    std::cerr << opt << std::endl ;
    return 1 ;
  }
  
  po::variables_map vm ;
  po::store(po::parse_command_line(argc , argv , opt) , vm) ;

  choi::GreedyPipe p(vm) ;
  p.set_signature(EXE);
  p.learn() ;
  return 0 ;
}

int test(int argc , char ** argv) {
  std::string usage = "usage" ;
  po::options_description opt = dp::build_greedy_test_optparser(usage) ;
  if(argc == 1) {
    std::cerr << opt << std::endl ;
    return 1 ;
  }

  po::variables_map vm ;
  po::store(po::parse_command_line(argc , argv , opt ) , vm) ;

  choi::GreedyPipe p(vm) ;
  p.test() ;
  return 0 ;
}

int main(int argc , char **argv) {
  std::string usage = "ZuoPar::" APP ".\n" ;
  usage += "Usage: " EXE "[learn|test] [options]" ;
  
  if(argc == 1) {
    std::cerr << usage << std::endl ;
    return 1 ;
  } else if(strcmp(argv[1],"learn") == 0) {
    return learn(argc - 1  , argv + 1) ;
  } else if(strcmp(argv[1] , "test") == 0) {
    return test(argc -1 , argv + 1) ;
  } else {
    std::cerr << usage << std::endl ;
    return 1 ;
  }
  return 0;
}
