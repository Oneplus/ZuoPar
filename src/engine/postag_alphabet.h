#ifndef __ZGEN_ENGINE_POSTAG_ALPHABET_H__
#define __ZGEN_ENGINE_POSTAG_ALPHABET_H__

#include "alphabet.h"

namespace ZGen {

namespace Engine {

class PennTreebankPostagAlphabet: public InmutableAlphabet {
public:
  /**
   * Get the Postag name for the index.
   *
   *  @param[in]  id          The encoded index.
   *  @return     const char* The name for the postag.
   */
  const char* decode(int id) const;

  /**
   * Get the index for the certain name.
   *
   *  @param[in]  name        The name.
   *  @return     int         The encoded index.
   */
  int encode(const char* name) const;

  //
  static const char * PENN_POS_NAME[];

  //
  enum PENN_POS_ID {
    NONE = 0,   BEGIN,      END,
    DOLLAR,     L_QUOTE,    R_QUOTE,
    L_BRACKET,  R_BRACKET,  COMMA,
    PERIOD,     COLUM,      SHARP,
    CC, CD, DT, EX, FW,
    IN, JJ, JJR, JJS, LS,
    MD, NN, NNP, NNPS, NNS, NP,
    PDT, POS, PRP, PRP_DOLLAR,
    RB, RBR, RBS, RP,
    SYM, TO, UH,
    VB, VBD, VBG, VBN, VBP, VBZ,
    WDT, WP, WP_DOLLAR, WRB, kMaxIndexOfPosTag,
  };
};

}   //  end for namespace Engine

}   //  end for namespace ZGen

#endif  //  end for __ZGEN_ENGING_POSTAG_ALPHABET_H__
