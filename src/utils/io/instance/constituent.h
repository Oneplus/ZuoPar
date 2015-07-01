#ifndef __ZUOPAR_UTILS_IO_INSTANCE_CONSTITUENT_H__
#define __ZUOPAR_UTILS_IO_INSTANCE_CONSTITUENT_H__

#include <iostream>
#include "types/constituent.h"
#include "engine/token_alphabet.h"

namespace ZuoPar {
namespace IO {

bool read_binarized_cfg_instance(std::istream& is,
    BinarizedCFGTree& output,
    Engine::TokenAlphabet& forms_alphabet,
    Engine::TokenAlphabet& postags_alphabet,
    Engine::TokenAlphabet& deprels_alphabet,
    size_t flag = 0xffff);

} //  namespace io
} //  namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_INSTANCE_CONSTITUENT_H__
