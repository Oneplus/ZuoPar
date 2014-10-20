#ifndef __ZGEN_ENGINE_DEPREL_ALPHABET_H__
#define __ZGEN_ENGINE_DEPREL_ALPHABET_H__

#include "alphabet.h"

namespace ZGen {

namespace Engine {

class PennTreebankDeprelAlphabet: public InmutableAlphabet {
public:
  /**
   * Get the dependency relation name for the index.
   *
   *  @param[in]  id          The encoded index.
   *  @return     const char* The name for the postag.
   */
  const char* decode(int id) const;

  /**
   * Get the index for the dependency relation name.
   *
   *  @param[in]  name        The name.
   *  @return     int         The encoded index.
   */
  int encode(const char* name) const;

  static const char * PENN_REL_NAME[];

  enum PENN_REL_ID {
    NONE = 0, AMOD, DEP, NMOD, OBJ,
    P,        PMOD, PRD, SBAR, SUB,
    VC,       VMOD, kMaxIndexOfDependencyRelation
  };

};

}

}

#endif  //  end for __ZGEN_ENGINE_DEPREL_ALPHABET_H__
