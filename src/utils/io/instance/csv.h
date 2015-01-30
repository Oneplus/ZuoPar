#ifndef __ZUOPAR_UTILS_IO_INSTANCE_CSV_H__
#define __ZUOPAR_UTILS_IO_INSTANCE_CSV_H__

#include <iostream>
#include <vector>

namespace ZuoPar {
namespace IO {

void read_csv(std::istream& is,
    std::vector< std::vector<std::string> >& output);

void write_csv(std::ostream& os,
    const std::vector< std::vector<std::string> >& output,
    const std::string& delimiter = "\t");

} //  namespace io
} //  namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_INSTANCE_CSV_H__
