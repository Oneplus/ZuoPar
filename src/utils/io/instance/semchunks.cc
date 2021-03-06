#include "utils/io/instance/semchunks.h"
#include "utils/io/instance/csv.h"
#include "utils/logging.h"
#include <vector>

namespace ZuoPar {
namespace IO {

void read_semchunks_instance(std::istream& is,
    SemanticChunks& output,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& senses_alphabet,
    eg::TokenAlphabet& chunks_alphabet,
    const std::string& predicate_tag,
    bool incremental) {
  std::vector< std::vector<std::string> > mat;
  read_csv(is, mat);

  int M = mat.size();
  int N = mat[0].size();
  for (int i = 0; i < M; ++ i) {
    form_t form = (incremental?
        forms_alphabet.insert(mat[i][0]):
        forms_alphabet.encode(mat[i][0]));

    postag_t postag = (incremental ?
        postags_alphabet.insert(mat[i][1]):
        postags_alphabet.encode(mat[i][1]));

    tag_t sense = (incremental?
        senses_alphabet.insert(mat[i][3]):
        senses_alphabet.encode(mat[i][3]));

    output.forms.push_back(form);
    output.postags.push_back(postag);
    output.senses.push_back(sense);
  }

  for (int i = 4; i < N; ++ i) {
    // N - 3's predicate.
    SemanticChunks::predicate_t predicate;
    predicate.first = -1;
    for (int j = 0; j < M; ++ j) {
      tag_t tag;
      if (mat[j][i] == "O") {
        tag = kSemanticChunkOuterTag;
      } else {
        BOOST_ASSERT_MSG(mat[j][i][1] == '-',
            "There should be a conjunction between boundary tag and chunk tag");

        if (mat[j][i][0] == 'B') {
          tag = kSemanticChunkBeginTag;
        } else if (mat[j][i][0] == 'I') {
          tag = kSemanticChunkInterTag;
        } else {
          BOOST_ASSERT_MSG(false, "utils::io::read_semchunk Unknown tag!");
        }

        std::size_t len = mat[j][i].size();
        tag += (incremental ?
            chunks_alphabet.insert(mat[j][i].substr(2, len - 2)):
            chunks_alphabet.encode(mat[j][i].substr(2, len - 2))
            );

        if (mat[j][i].substr(2) == predicate_tag) { predicate.first = j; }
      }
      predicate.second.push_back(tag);
    }
    BOOST_ASSERT_MSG(predicate.first != -1, "predicate not found!");
    output.predicates.push_back(predicate);
  }
}

void write_semchunks_instance(std::ostream& os,
    const SemanticChunks& output,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& senses_alphabet,
    const eg::TokenAlphabet& chunks_alphabet) {
  std::vector< std::vector<std::string> > mat;
  int N = 4 + output.nr_predicates();
  int M = output.size();

  mat.resize(M);
  for (int i = 0; i < M; ++ i) {
    mat[i].resize(N, "");
  }

  for (int i = 0; i < M; ++ i) {
    mat[i][0] = forms_alphabet.decode(output.forms[i]);
    mat[i][1] = postags_alphabet.decode(output.postags[i]);
    mat[i][2] = "-";
    mat[i][3] = senses_alphabet.decode(output.senses[i]);
  }

  for (int j = 0; j < output.nr_predicates(); ++ j) {
    const SemanticChunks::predicate_t& predicate = output.predicates[j];
    for (int i = 0; i < predicate.second.size(); ++ i) {
      if (predicate.second[i] == kSemanticChunkOuterTag) {
        mat[i][j+ 4] = "O";
      } else if (predicate.second[i] >= kSemanticChunkBeginTag
          && predicate.second[i] < kSemanticChunkInterTag) {
        mat[i][j+ 4] = "B-";
        mat[i][j+ 4] += chunks_alphabet.decode(predicate.second[i]- kSemanticChunkBeginTag);
      } else if (predicate.second[i] >= kSemanticChunkInterTag) {
        mat[i][j+ 4] = "I-";
        mat[i][j+ 4] += chunks_alphabet.decode(predicate.second[i]- kSemanticChunkInterTag);
      } else {
        BOOST_ASSERT_MSG(false, "Unrecognized semantic chunk boundary!");
      }
    }
  }

  write_csv(os, mat);
}

void write_props_instance(std::ostream& os,
    const SemanticChunks& output,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& senses_alphabet,
    const eg::TokenAlphabet& chunks_alphabet) {
  if (output.nr_predicates() == 0) {
    return;
  }

  std::vector< std::vector<std::string> > mat;
  int N = 1 + output.nr_predicates();
  int M = output.size();

  mat.resize(M);
  for (int i = 0; i < M; ++ i) { mat[i].resize(N, "*"); }

  for (int i = 0; i < M; ++ i) {
    mat[i][0] = senses_alphabet.decode(output.senses[i]);
    if (mat[i][0] == "_") { mat[i][0] = "-"; }
  }

  for (int j = 0; j < output.nr_predicates(); ++ j) {
    const SemanticChunks::predicate_t& predicate = output.predicates[j];

    int i;
    for (i = 0; i < predicate.second.size(); ++ i) {
      if (predicate.second[i] < kSemanticChunkInterTag) {
        if (i- 1 >= 0 && predicate.second[i- 1] > kSemanticChunkBeginTag) {
          mat[i-1][j+ 1] = mat[i-1][j+ 1] + ")";
        }

        if (predicate.second[i] > kSemanticChunkBeginTag) {
          mat[i][j+ 1] = (chunks_alphabet.decode(predicate.second[i]- kSemanticChunkBeginTag)
              + mat[i][j+ 1]);
          mat[i][j+ 1] = ("(" + mat[i][j+ 1]);
        }
      }
    }
    if (predicate.second[i- 1] > kSemanticChunkBeginTag) {
      mat[i- 1][j+ 1] = mat[i- 1][j+ 1]+ ")";
    }
  }

  write_csv(os, mat);
}

} //  namespace io
} //  namespace zuopar
