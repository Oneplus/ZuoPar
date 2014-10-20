#ifndef __ZGEN_ENGINE_ACTION_ALPHABET_H__
#define __ZGEN_ENGINE_ACTION_ALPHABET_H__

#include "alphabet.h"

namespace ZGen {

namespace Engine {

class ActionAlphabet: public InmutableAlphabet {
public:
  /**
   * Get the action name for the index.
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
  static const char* ACTION_NAME[];

  enum ACTION_ID {
    NONE = 0, LA, RA, SH, kMaxIndexOfAction
  };
};

} //  end for namespace Engine

} //  end for namespace ZGen



#endif  //  end for __ZGEN_ENGINE_ACTION_ALPHABET_H__
