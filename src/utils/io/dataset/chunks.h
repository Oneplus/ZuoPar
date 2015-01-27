#ifndef __ZUOPAR_UTILS_IO_DATASET_CHUNKS_H__
#define __ZUOPAR_UTILS_IO_DATASET_CHUNKS_H__

#include <iostream>
#include <vector>
#include "types/chunks.h"
#include "utils/io/instance/chunks.h"
#include "engine/token_alphabet.h"

namespace ZuoPar {
namespace IO {

namespace eg = ZuoPar::Engine;

/**
 * Read one semantic chunk with chunk tag instance from the input stream.
 *
 *  @param[in]  is                The input stream
 *  @param[out] dataset           The output dataset
 *  @param[in]  forms_alphabet    The alphabet for forms
 *  @param[in]  postags_alphabet  The alphabet for postags
 *  @param[in]  senses_alphabet   The alphabet for senses
 *  @param[in]  chunks_alphabet   The alphabet for chunks
 *  @param[in]  semroles_alphabet The alphabet for semantic roles
 *  @param[in]  predicate_tag     The string tag for predicate
 *  @param[in]  incremental       Use to specify if add new key entry to the
 *                                alphabet.
 */
void read_chunks_with_semantic_roles_dataset(std::istream& is,
    std::vector<ChunksWithSemanticRoles>& dataset,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& senses_alphabet,
    eg::TokenAlphabet& chunks_alphabet,
    eg::TokenAlphabet& semroles_alphabet,
    const std::string& predicate_tag = "V",
    bool incremental = true);

} //  namespace io
} //  namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_DATASET_CHUNKS_H__
