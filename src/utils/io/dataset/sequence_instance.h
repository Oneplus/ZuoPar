#ifndef __ZUOPAR_UTILS_IO_DATASET_SEQUENCE_INSTANCE_H__
#define __ZUOPAR_UTILS_IO_DATASET_SEQUENCE_INSTANCE_H__

#include <iostream>
#include <vector>
#include "types/sequence_instance.h"
#include "utils/io/instance/sequence_instance.h"
#include "engine/token_alphabet.h"

namespace ZuoPar {
namespace IO {

namespace eg = ZuoPar::Engine;

/**
 * Read dependency dataset. In each instance, one item per line. For each item,
 * there are multi-column: the first column represents the tag, the rest columns
 * represents the string featuers. Instances are separated by newline.
 *
 *  @param[in]  is                    The input stream.
 *  @param[out] dataset               The output dataset.
 *  @param[out] attributes_alphabets  The alphabet for the attributes.
 *  @param[out] tags_alphabet         The alphabet for tags.
 *  @param[in]  incremental           If incremental is true, insert the token into
 *                                    the alphabet, otherwise not perform insertion.
 */
void read_sequence_instance_dataset(
    std::istream& is,
    std::vector<SequenceInstance>& dataset,
    eg::TokenAlphabet& attributes_alphabets,
    eg::TokenAlphabet& tags_alphabet,
    bool incremental = true);

}
}

#endif  //  end for __ZUOPAR_UTILS_IO_DATASET_H__
