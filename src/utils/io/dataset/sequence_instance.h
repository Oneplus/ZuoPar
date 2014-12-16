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
 * there are 4 column representing: form, postag, head, deprel. Instances are
 * separated by newline.
 *
 *  @param[in]  is                The input stream.
 *  @param[out] dataset           The output dataset.
 *  @param[out] forms_alphabet    The alphabet for forms.
 *  @param[out] postags_alphabet  The alphabet for postags.
 *  @param[out] deprels_alphabet  The alphabet for dependency relations.
 *  @param[in]  incremental       If incremental is true, insert the token into
 *                                the alphabet, otherwise not perform insertion.
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
