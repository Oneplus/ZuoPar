#ifndef __ZUOPAR_UTILS_IO_DATASET_CONSTITUENT_H__
#define __ZUOPAR_UTILS_IO_DATASET_CONSTITUENT_H__

#include <iostream>
#include <vector>
#include "types/constituent.h"
#include "engine/token_alphabet.h"

namespace ZuoPar {
namespace IO {

void read_binarized_cfg_dataset(std::istream& is,
    std::vector<BinarizedCFGTree>& dataset,
    Engine::TokenAlphabet& forms_alphabet,
    Engine::TokenAlphabet& postags_alphabet,
    Engine::TokenAlphabet& constituents_alphabet,
    size_t flag = 0xffff);

} //  namespace io
} //  namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_DATASET_CONSTITUENT_H__
