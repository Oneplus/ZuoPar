#ifndef __ZUOPAR_UTILS_IO_INSTANCE_CHUNKS_H__
#define __ZUOPAR_UTILS_IO_INSTANCE_CHUNKS_H__

#include "engine/token_alphabet.h"
#include "types/chunks.h"

namespace ZuoPar {
namespace IO {

namespace eg = ZuoPar::Engine;

/**
 * Read one semantic chunk with chunk tag instance from the input stream.
 *
 *  @param[in]  is                The input stream
 *  @param[out] output            The output dependency parse tree
 *  @param[in]  forms_alphabet    The alphabet for forms
 *  @param[in]  postags_alphabet  The alphabet for postags
 *  @param[in]  senses_alphabet   The alphabet for senses
 *  @param[in]  chunks_alphabet   The alphabet for chunk
 *  @param[in]  semroles_alphabet The alphabet for semantic role
 *  @param[in]  incremental       Use to specify if add new key entry to the
 *                                alphabet.
 */
void read_chunks_with_semantic_roles_instance(std::istream& is,
    ChunksWithSemanticRoles& output,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& senses_alphabet,
    eg::TokenAlphabet& chunks_alphabet,
    eg::TokenAlphabet& semroles_alphabet,
    const std::string& predicate_tag,
    bool incremental);

void write_chunks_with_semantic_chunks_instance(std::ostream& is,
    const ChunksWithSemanticRoles& input,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& senses_alphabet,
    const eg::TokenAlphabet& chunks_alphabet,
    const eg::TokenAlphabet& semroles_alphabet);

void write_chunks_with_semantic_props_instance(std::ostream& os,
    const ChunksWithSemanticRoles& input,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& senses_alphabet,
    const eg::TokenAlphabet& chunks_alphabet,
    const eg::TokenAlphabet& semroles_alphabet);

} //  namespace io
} //  namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_INSTANCE_CHUNKS_H__
