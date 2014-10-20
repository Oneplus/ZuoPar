#include "engine/deprel_alphabet.h"
#include <cstring>

namespace ZGen {

namespace Engine {

const char *
PennTreebankDeprelAlphabet::PENN_REL_NAME[]  = {
  "-NONE-", "AMOD", "DEP", "NMOD", "OBJ",
  "P",      "PMOD", "PRD", "SBAR", "SUB",
  "VC",     "VMOD",
};


const char *
PennTreebankDeprelAlphabet::decode(int id) const {
  if (id < 0 || id > kMaxIndexOfDependencyRelation) {
    return 0;
  }
  return PENN_REL_NAME[id];
}


int
PennTreebankDeprelAlphabet::encode(const char* name) const {
  for (int i = 0; i < kMaxIndexOfDependencyRelation; ++ i) {
    if (0== strcmp(PENN_REL_NAME[i], name)) {
      return static_cast<PENN_REL_ID>(i);
    }
  }
  return NONE;
}


}   //  end for namespace Engine

}   //  end for namespace ZGen
