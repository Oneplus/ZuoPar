#include <fstream>
#include <cstring>
#include "utils/io/stream.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace IO {

std::istream* get_istream(const char* filename, bool verbose,
    std::istream* default_stream) {
  std::istream* is = NULL;
  if (filename!= NULL && strlen(filename) > 0) {
    is = new std::ifstream(filename);
    if (!is->good()) {
      _WARN << "[IO] Opening file failure, use default stream";
      is = default_stream;
    } else {
      if (verbose)
        _INFO << "[IO] Opening file stream from "<< filename << ".";
    }
  } else {
    _WARN << "[IO] Input file not specified, use default stream";
    is = default_stream;
  }
  return is;
}

std::istream* get_istream(const std::string& filename, bool verbose,
    std::istream* default_stream) {
  return get_istream(filename.c_str(), verbose, default_stream);
}

std::ostream* get_ostream(const char* filename, bool verbose,
    std::ostream* default_stream) {
  std::ostream* os = NULL;
  if (filename!= NULL && strlen(filename) > 0) {
    os = new std::ofstream(filename);
    if (!os->good()) {
      _WARN << "[IO] Opening file failure, use default stream";
      os = default_stream;
    } else {
      if (verbose)
        _INFO << "[IO] File stream from: "<< filename << " is opened.";
    }
  } else {
    _WARN << "[IO] Output file not specified, use default stream";
    os = default_stream;
  }
  return os;
}

std::ostream* get_ostream(const std::string& filename, bool verbose,
    std::ostream* default_stream) {
  return get_ostream(filename.c_str(), verbose, default_stream);
}

size_t number_of_lines(std::istream& is) {
  const int size = 1024*1024;
  char buffer[1024*1024];
  size_t retval = 0;

  while (true) {
    is.read(buffer, 1024*1024);
    auto cc = is.gcount();
    if (0 == cc) { break; }
    for (auto i = 0; i < cc; ++ i) { if (buffer[i] == '\n') { ++ retval; } }
  }
  return retval;
}

} //  end for io
} //  end for zuopar
