#ifndef __ZUOPAR_UTILS_IO_DATASET_SEGMENTATION_H__
#define __ZUOPAR_UTILS_IO_DATASET_SEGMENTATION_H__

#include <iostream>
#include <vector>
#include "types/segmentation.h"
#include "utils/io/instance/segmentation.h"

namespace ZuoPar {
namespace IO {

/**
 * Read segmentation dataset. In each instance, words are separated by space.
 *
 *  @param[in]  is                          The input stream.
 *  @param[out] dataset                     The output dataset.
 *  @param[in]  incremental                 Specify to insert token into
 *                                          alphabet.
 */
void read_segmentation_dataset(std::istream& is, std::vector<Segmentation>& dataset,
    bool segmented);

} //  end for io
} //  end for zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_DATASET_SEGMENTATION_H__
