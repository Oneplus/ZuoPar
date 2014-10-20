#ifndef __ZUOPAR_ENGINE_ENGINE_H__
#define __ZUOPAR_ENGINE_ENGINE_H__

#include "action_alphabet.h"
#include "postag_alphabet.h"
#include "deprel_alphabet.h"
#include "word_alphabet.h"

namespace ZuoPar {

namespace Engine {

typedef boost::serialization::singleton<PennTreebankPostagAlphabet> PostagEngine;
typedef boost::serialization::singleton<PennTreebankDeprelAlphabet> DeprelEngine;
typedef boost::serialization::singleton<WordAlphabet> WordEngine;
typedef boost::serialization::singleton<ActionAlphabet> ActionEngine;

} //  end for Engine

} //  end for ZuoPar

#endif  //  end for __ZUOPAR_ENGINE_ENGINE_H__

