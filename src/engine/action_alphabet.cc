#include <cstring>
#include "engine/action_alphabet.h"

namespace ZGen {

namespace Engine {

// The action name encoder/decoder
const char *
ActionAlphabet::ACTION_NAME[] = {
  "-NONE-", "LA", "RA", "SH",
};


const char *
ActionAlphabet::decode(int id) const {
  if (id < 0 || id > kMaxIndexOfAction) {
    return 0;
  }
  return ACTION_NAME[id];
}

int
ActionAlphabet::encode(const char * name) const {
  for (int i = 0; i < kMaxIndexOfAction; ++ i) {
    if (0== strcmp(ACTION_NAME[i], name)) {
      return static_cast<ACTION_ID>(i);
    }
  }

  return NONE;
}

} //  end for namespace Engine

} //  end for namespace ZGen
