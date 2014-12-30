#ifndef __ZUOPAR_LOGGING_HPP__
#define __ZUOPAR_LOGGING_HPP__

#include <boost/log/trivial.hpp>
#define _TRACE BOOST_LOG_TRIVIAL(trace)
#define _DEBUG BOOST_LOG_TRIVIAL(debug)
#define _INFO  BOOST_LOG_TRIVIAL(info)
#define _WARN  BOOST_LOG_TRIVIAL(warning)
#define _ERROR BOOST_LOG_TRIVIAL(error)

namespace ZuoPar {
namespace Utility {

void init_boost_log(bool verbose);

} //  end for utility
} //  end for zuopar

#endif  //  end for __ZUOPAR_LOGGING_HPP__
