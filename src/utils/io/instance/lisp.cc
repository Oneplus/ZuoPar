//#include "utils/io/instance/lisp.h"
#include "lisp.h"
#include <list>

namespace ZuoPar {
namespace IO {

std::list<std::string> _read_lisp_tokenize(const std::string& line) {
  std::list<std::string> tokens;
  const char* s = line.c_str();
  while (*s) {
    while (*s == ' ') { ++ s; }
    if (*s == '(' || *s == ')') {
      tokens.push_back(*s++ == '(' ? "(" : ")");
    } else {
      const char* t = s;
      while (*t && *t != ' ' && *t != '(' && *t != ')') { ++ t; }
      tokens.push_back(std::string(s, t));
      s = t;
    }
  }
  return tokens;
}

LispCell _read_lisp_atom(const std::string& token) {
  LispCell c;
  c.val = token;
  return c;
}

LispCell _read_lisp_from(std::list<std::string>& tokens) {
  const std::string token(tokens.front());
  tokens.pop_front();
  if (token == "(") {
    LispCell c;
    while (tokens.front() != ")") {
      c.list.push_back(_read_lisp_from(tokens));
    }
    tokens.pop_front();
    return c;
  } else {
    return _read_lisp_atom(token);
  }
}

LispCell read_lisp(const std::string& line) {
  std::list<std::string> tokens(_read_lisp_tokenize(line));
  return _read_lisp_from(tokens);
}

} //  namespace io
} //  namespace zuopar
