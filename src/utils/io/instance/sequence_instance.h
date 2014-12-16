#ifndef __ZUOPAR_UTILS_IO_INSTANCE_SEQUENCE_INSTANCE_H__
#define __ZUOPAR_UTILS_IO_INSTANCE_SEQUENCE_INSTANCE_H__

#include <iostream>
#include "types/sequence_instance.h"
#include "engine/token_alphabet.h"

namespace ZuoPar {
namespace IO {

namespace eg = ZuoPar::Engine;

/**
 * Read one segmentation instance from the input stream.
 *
 *  @param[in]  is                The input stream.
 *  @param[out] output            The output stream.
 *  @param[in]  forms_alphabet    The alphabet of forms.
 *  @param[in]  postags_alphabet  The alphabet of postags.
 *  @return     bool              Return true on successfully load, otherwise
 *                                false.
 */
bool read_sequence_instance(std::istream& is,
    SequenceInstance& output,
    eg::TokenAlphabet& attributes_alphabet,
    eg::TokenAlphabet& tags_alphabet,
    bool incremental = true);


/**
 *
 *
 *
 */
void write_sequence_instance(std::ostream& os,
    const SequenceInstance& output,
    const eg::TokenAlphabet& attributes_alphabet,
    const eg::TokenAlphabet& tags_alphabet);

} //  end for namespace io
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_INSTANCE_SEQUENCE_INSTANCE_H__
