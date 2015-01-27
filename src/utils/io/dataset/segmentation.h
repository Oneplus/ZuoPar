#ifndef __ZUOPAR_UTILS_IO_DATASET_SEGMENTATION_H__
#define __ZUOPAR_UTILS_IO_DATASET_SEGMENTATION_H__

#include <iostream>
#include <vector>
#include "types/segmentation.h"
#include "utils/io/instance/segmentation.h"
#include "engine/token_alphabet.h"

namespace ZuoPar {
namespace IO {

namespace eg = ZuoPar::Engine;

/**
 * Read segmentation dataset. In each instance, words are separated by space.
 *
 *  @param[in]  is                          The input stream.
 *  @param[out] dataset                     The output dataset.
 *  @param[out] characters_alphabet         The alphabet for characters
 *  @param[in]  incremental                 Specify to insert token into
 *                                          alphabet.
 */
void read_segmentation_dataset(
    std::istream& is,
    std::vector<Segmentation>& dataset,
    eg::TokenAlphabet& characters_alphabet,
    bool incremental = true);

} //  end for io
} //  end for zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_DATASET_SEGMENTATION_H__
