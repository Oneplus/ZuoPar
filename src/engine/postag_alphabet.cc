#include <cstring>
#include "engine/postag_alphabet.h"

namespace ZGen {

namespace Engine {

const char *
PennTreebankPostagAlphabet::PENN_POS_NAME[] = {
   "-NONE-",
   "-BEGIN-",
   "-END-",
   "$", "``", "''",
   "-LRB-", "-RRB-", ",", // note that brackets can sometimes be ( and )
   ".", ":", "#",
   "CC", "CD", "DT", "EX", "FW",
   "IN", "JJ", "JJR", "JJS", "LS",
   "MD", "NN", "NNP", "NNPS", "NNS", "NP",
   "PDT", "POS", "PRP", "PRP$",
   "RB", "RBR", "RBS", "RP",
   "SYM", "TO", "UH",
   "VB", "VBD", "VBG", "VBN", "VBP", "VBZ",
   "WDT", "WP", "WP$", "WRB"
};

const char *
PennTreebankPostagAlphabet::decode(int id) const {
  if (id < 0 || id > kMaxIndexOfPosTag) {
    return 0;
  }
  return PENN_POS_NAME[id];
}

int
PennTreebankPostagAlphabet::encode(const char* name) const {
  for (int i = 0; i < kMaxIndexOfPosTag; ++ i) {
    if (0== strcmp(PENN_POS_NAME[i], name)) {
      return static_cast<PENN_POS_ID>(i);
    }
  }
  return NONE;
}


}   //  end for namespace Engine.
}   //  end for namespace ZGen

