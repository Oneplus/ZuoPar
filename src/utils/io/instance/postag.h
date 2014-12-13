#ifndef __ZUOPAR_UTILS_IO_INSTANCE_POSTAG_H__
#define __ZUOPAR_UTILS_IO_INSTANCE_POSTAG_H__

#include <iostream>
#include "types/postag.h"
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
bool read_postag_instance(std::istream& is,
    Postag& output,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    char delimiter = '/',
    bool incremental = true);


/**
 *
 *
 *
 */
void write_postag_instance(std::ostream& os,
    const Postag& output,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    char delimiter = '/');

/**
 * Read one postag instance from the input stream.
 *
 *  @param[in]  is                The input stream.
 *  @param[out] output            The output stream.
 *  @param[in]  postags_alphabet  The alphabet of postags.
 *  @param[in]  delimiter         The delimiter between form and postag.
 *  @param[in]  incremental
 *  @return     bool              Return true on successfully load, otherwise
 *                                false.
 */
bool read_postag_with_cache_instance(std::istream& is,
    PostagWithLiteralCache& output,
    eg::TokenAlphabet& postags_alphabet,
    char delimiter = '/',
    bool incremental = true);

/**
 *
 *
 *
 */
void write_postag_with_cache_instance(std::ostream& os,
    const PostagWithLiteralCache& output,
    const eg::TokenAlphabet& postags_alphabet,
    char delimiter = '/');

} //  end for namespace io
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_INSTANCE_POSTAG_H__
