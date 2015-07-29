#include <iostream>
#include <cstring>
#include <boost/program_options.hpp>
#include "app/depparser/greedy_opt_utils.h"
#include "app/depparser/swap/pipe.h"

#define APP "swap-based greedy dependency parser"
#define EXE "depparser_swap_greedy"

namespace po = boost::program_options ;
namespace sw = ZuoPar::DependencyParser::Swap ;
namespace dp = ZuoPar::DependencyParser ;

int learn(int argc , char ** argv)
{
    std::string usage = "usage" ;
    po::options_description opt = dp::build_greedy_learn_optparser(usage) ;
    if(argc == 1) 
    {
        std::cerr << opt << std::endl ;
        return 1 ;
    }

    po::variables_map vm ;
    po::store(po::parse_command_line(argc , argv , opt) , vm) ;
    dp::GreedyLearnOption rep ;
    if(!dp::parse_greedy_learn_option(vm,rep))
    {
        std::cerr << opt << std::endl ;
        return 1 ;
    }

    sw::GreedyPipe p(rep) ;
    p.learn() ;
    return 0 ;
}


int test(int argc , char ** argv)
{
    std::string usage = "usage" ;
    po::options_description opt = dp::build_greedy_test_optparser(usage) ;
    if(argc == 1)
    {
        std::cerr << opt << std::endl ;
        return 1 ;
    }

    po::variables_map vm ;
    po::store(po::parse_command_line(argc , argv , opt ) , vm) ;

    dp::GreedyTestOption rep ;
    if(!dp::parse_greedy_test_option(vm , rep))
    {   
        std::cerr << opt << std::endl ;
        return 1 ;
    }

    sw::GreedyPipe p(rep) ;
    p.test() ;
    return 0 ;
}

int main(int argc , char **argv)
{
    std::string usage = "ZuoPar::" APP ".\n" ;
    usage += "Usage: " EXE "[learn|test] [options]" ;

    if(argc == 1)
    {
        std::cerr << usage << std::endl ;
        return 1 ;
    }
    else if(strcmp(argv[1],"learn") == 0)
    {
        return learn(argc - 1  , argv + 1) ;
    }
    else if(strcmp(argv[1] , "test") == 0)
    {
        return test(argc -1 , argv + 1) ;
    }
    else 
    {
        std::cerr << usage << std::endl ;
        return 1 ;
    }
}
