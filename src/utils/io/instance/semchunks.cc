#include "utils/io/instance/semchunks.h"
#include <vector>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace ZuoPar {
namespace IO {

void
read_semchunks_instance(std::istream& is,
    SemanticChunks& output,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& chunks_alphabet,
    bool incremental) {
  namespace algo = boost::algorithm;
  std::vector< std::vector<std::string> > mat;
  std::string line;
  while (std::getline(is, line)) {
    algo::trim(line);
    if (line.size() == 0) { return; }
    std::vector<std::string> items;
    algo::split(items, line, boost::is_any_of("\t "), boost::token_compress_on);
    mat.push_back(items);
  }

  int M = mat.size();
  int N = mat[0].size();
  for (int i = 3; i < N; ++ i) {
    // N - 3's predicate.
    SemanticChunks::predicate_t predicate;
    predicate.first = -1;
    for (int j = 0; j < M; ++ j) {
      tag_t tag;
      if (mat[j][i] == "O") {
        tag = 0;
      } else {
        BOOST_ASSERT_MSG(mat[j][i][1] == '-', "Message");
        if (mat[j][i][0] == 'B') {
          tag = 1000000;
        } else if (mat[j][i][0] == 'I') {
          tag = 2000000;
        } else {
          BOOST_ASSERT(false);
        }
        std::size_t len = mat[j][i].size();
        tag += (incremental ?
            forms_alphabet.insert(mat[j][i].substr(2, len - 2).c_str()):
            forms_alphabet.encode(mat[j][i].substr(2, len - 2).c_str())
            );

        if (mat[j][i][2] == 'V' && mat[j][i][3] == 0) {
          // currently, only the 'B-V' is considered as the predicate.
          predicate.first = j;
        }
      }
      predicate.second.push_back(tag);
    }
    BOOST_ASSERT_MSG(predicate.first != -1, "not equals to ");
  }
}

void
write_semchunks_instance(std::ostream& os,
    const SemanticChunks& output,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& chunks_alphabet) {
}


} //  namespace io
} //  namespace zuopar
