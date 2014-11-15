#ifndef __ZUOPAR_UTILS_IO_DATASET_H__
#define __ZUOPAR_UTILS_IO_DATASET_H__

#include <iostream>
#include <vector>
#include "types/dependency.h"
#include "engine/token_alphabet.h"

namespace eg = ZuoPar::Engine;

namespace ZuoPar {
namespace IO {

/**
 * Read plain dependency dataset
 *
 *  @param[in]  is                The 
 *  @param[out] dataset           The output dataset.
 *  @param[out] forms_alphabet    The
 *  @param[out] postags_alphabet  The postag 
 *  @param[out] deprels_alphabet
 */
void read_plain_dependency_dataset(
    std::istream& is,
    std::vector<Dependency>& dataset,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& deprels_alphabet,
    bool incremental = true);

}
}

#endif  //  end for __ZUOPAR_UTILS_IO_DATASET_H__
