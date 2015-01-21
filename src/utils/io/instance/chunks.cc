#include "utils/io/instance/chunks.h"
#include "utils/logging.h"
#include <vector>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace ZuoPar {
namespace IO {

void
read_chunks_with_semantic_roles_instance(std::istream& is,
    ChunksWithSemanticRoles& output,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& senses_alphabet,
    eg::TokenAlphabet& chunks_alphabet,
    eg::TokenAlphabet& semroles_alphabet,
    const std::string& predicate_tag,
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
  for (int i = 0; i < M; ++ i) {
    form_t form = (incremental ?
        forms_alphabet.insert(mat[i][0].c_str()):
        forms_alphabet.encode(mat[i][0].c_str())
        );
    postag_t postag = (incremental ?
        postags_alphabet.insert(mat[i][1].c_str()):
        postags_alphabet.encode(mat[i][1].c_str())
        );
    tag_t sense = (incremental?
        senses_alphabet.insert(mat[i][3].c_str()):
        senses_alphabet.encode(mat[i][3].c_str())
        );

    output.forms.push_back(form);
    output.postags.push_back(postag);
    output.senses.push_back(sense);

    tag_t chunk = 0;
    if (mat[i][2] == "O") {
      chunk = kChunkOuterPrefix;
    } else {
      BOOST_ASSERT_MSG(mat[i][2][1] == '-',
          "There should be a conjunction between chunk boundary and chunk type.");
      if (mat[i][2][0] == 'B')      { chunk = kChunkBeginPrefix; }
      else if (mat[i][2][0] == 'I') { chunk = kChunkInterPrefix; }
      else if (mat[i][2][0] == 'E') { chunk = kChunkEndPrefix; }
      else if (mat[i][2][0] == 'C') { chunk = kChunkSinglePrefix; }
      else { BOOST_ASSERT_MSG(false, "Unrecognized chunk boundary tag."); }

      chunk += (incremental?
          chunks_alphabet.insert(mat[i][2].substr(2).c_str()):
          chunks_alphabet.insert(mat[i][2].substr(2).c_str())
          );
    }
    output.chunks.push_back(chunk);
  }

  for (int i = 4; i < N; ++ i) {
    // N - 3's predicate.
    ChunksWithSemanticRoles::predicate_t predicate;
    predicate.first = -1;
    for (int j = 0; j < M; ++ j) {
      tag_t tag;
      if (mat[j][i] == "O" || mat[j][i] == "-") {
        tag = 0;
      } else {
        BOOST_ASSERT_MSG(mat[j][2][0] == 'E' || mat[j][2][0] == 'C' || mat[j][2][0] == 'O',
            "Semantic role is labeled on a non-end tag.");
        tag = (incremental ?
            semroles_alphabet.insert(mat[j][i].c_str()):
            semroles_alphabet.encode(mat[j][i].c_str())
            );
        if (mat[j][i] == predicate_tag) { predicate.first = j; }
      }
      predicate.second.push_back(tag);
    }
    BOOST_ASSERT_MSG(predicate.first != -1, "predicate not found!");
    output.predicates.push_back(predicate);
  }
}

void
write_chunks_with_semantic_chunks_instance(std::ostream& is,
    const ChunksWithSemanticRoles& input,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& senses_alphabet,
    const eg::TokenAlphabet& chunks_alphabet,
    const eg::TokenAlphabet& semroles_alphabet) {
}

void
write_chunks_with_semantic_props_instance(std::ostream& os,
    const ChunksWithSemanticRoles& input,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& senses_alphabet,
    const eg::TokenAlphabet& chunks_alphabet,
    const eg::TokenAlphabet& semroles_alphabet) {
}

} //  namespace io
} //  namespace zuopar
