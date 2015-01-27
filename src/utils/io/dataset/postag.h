#ifndef __ZUOPAR_UTILS_IO_DATASET_POSTAG_H__
#define __ZUOPAR_UTILS_IO_DATASET_POSTAG_H__

#include <iostream>
#include <vector>
#include "types/postag.h"
#include "utils/io/instance/postag.h"
#include "engine/token_alphabet.h"

namespace ZuoPar {
namespace IO {

namespace eg = ZuoPar::Engine;

/**
 * Read segmentation dataset. In each instance, words are separated by space.
 *
 *  @param[in]  is                The input stream.
 *  @param[out] dataset           The output dataset.
 *  @param[out] forms_alphabet    The alphabet for forms.
 *  @param[out] postags_alphabet  The alphabet for postags.
 *  @param[in]  delimiter         The delimiter for separating form and postag.
 *  @param[in]  incremental       Specify to insert token into alphabet.
 */
void read_postag_dataset(
    std::istream& is,
    std::vector<Postag>& dataset,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    char delimiter = '/',
    bool incremental = true);

/**
 * Read segmentation dataset. In each instance, words are separated by space.
 *
 *  @param[in]  is                The input stream.
 *  @param[out] dataset           The output dataset.
 *  @param[in]  postags_alphabet  The alphabet for postags.
 *  @param[in]  delimiter         The delimiter for separating form and postag.
 *  @param[in]  incremental       Specify to insert token into alphabet.
 */
void read_postag_with_cache_dataset(
    std::istream& is,
    std::vector<PostagWithLiteralCache>& dataset,
    eg::TokenAlphabet& postags_alphabet,
    char delimiter = '/',
    bool incremental = true);

} //  end for io
} //  end for zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_DATASET_POSTAG_H__
