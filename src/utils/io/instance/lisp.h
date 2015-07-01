#ifndef __ZUOPAR_UTILS_IO_INSTANCE_LISP_H__
#define __ZUOPAR_UTILS_IO_INSTANCE_LISP_H__

#include <vector>
#include <string>

namespace ZuoPar {
namespace IO {

struct LispCell {
  std::vector<LispCell> list;
  std::string val;

  size_t size() const { return list.size(); }
};

LispCell read_lisp(const std::string& line);

} //  namespace io
} //  namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_INSTANCE_LISP_H__
