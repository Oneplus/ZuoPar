#ifndef __ZUOPAR_UTILS_IO_SEMANTIC_CHUNKS_H__
#define __ZUOPAR_UTILS_IO_SEMANTIC_CHUNKS_H__

#include "engine/token_alphabet.h"
#include "types/semchunks.h"

namespace ZuoPar {
namespace IO {

namespace eg = ZuoPar::Engine;

/**
 * Read one semantic chunk instance from the input stream.
 *
 *  @param[in]  is                The input stream
 *  @param[out] output            The output dependency parse tree
 *  @param[in]  forms_alphabet    The alphabet for forms
 *  @param[in]  postags_alphabet  The alphabet for postags
 *  @param[in]  deprels_alphabet  The alphabet for dependency relation
 *  @param[in]  incremental       Use to specify if add new key entry to the
 *                                alphabet.
 */
void read_semchunks_instance(std::istream& is,
    SemanticChunks& output,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& chunks_alphabet,
    bool incremental = true);

/**
 * Write one dependency instance to the output stream.
 *
 *  @param[out] os                The output stream
 *  @param[in]  output            The instance.
 *  @param[in]  forms_alphabet    The alphabet for forms
 *  @param[in]  postags_alphabet  The alphabet for postags
 *  @param[in]  deprels_alphabet  The alphabet for dependency relation
 */
void write_semchunks_instance(std::ostream& os,
    const SemanticChunks& output,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& chunks_alphabet);

} //  namespace io
} //  namespace zuopar


#endif  //  end for __ZUOPAR_UTILS_IO_SEMANTIC_CHUNKS_H__
