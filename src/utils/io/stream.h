#ifndef __ZUOPAR_UTILS_IO_STREAM_H__
#define __ZUOPAR_UTILS_IO_STREAM_H__

#include <iostream>

namespace ZuoPar {
namespace IO {

/**
 * Get the ifstream from the file specified by filename. If opening fails,
 * use the provided default stream instead.
 *
 *  @param[in]  filename        The name of the file.
 *  @param[in]  default_stream  The user specified stream.
 *  @return     std::istream    The built stream.
 */
std::istream* get_istream(const char* filename,
    std::istream* default_stream = (&(std::cin)));

//! std::string wrapper for get_istream
std::istream* get_istream(const std::string& filename,
    std::istream* default_stream = (&(std::cin)));

/**
 * Get the ostream from the file specified by filename. If opening fails,
 * use the provided default stream instead.
 *
 *  @param[in]  filename        The name of the file.
 *  @param[in]  default_stream  The user specified stream.
 *  @return     std::ostream    The built stream.
 */
std::ostream* get_ostream(const char* filename,
    std::ostream* default_stream = (&(std::cout)));

//! std::string wrapper for get_ostream.
std::ostream* get_ostream(const std::string& filename,
    std::ostream* default_stream = (&(std::cout)));

} //  end for io
} //  end for zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_STREAM_H__
