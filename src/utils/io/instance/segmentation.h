#ifndef __ZUOPAR_UTILS_IO_INSTANCE_SEGMENTATION_H__
#define __ZUOPAR_UTILS_IO_INSTANCE_SEGMENTATION_H__

#include <iostream>
#include "types/segmentation.h"
#include "engine/token_alphabet.h"

namespace ZuoPar {
namespace IO {

namespace eg = ZuoPar::Engine;

/**
 * Read one segmentation instance from the input stream.
 *
 *  @param[in]  is                  The input stream.
 *  @param[out] output              The output stream.
 *  @param[in]  character_alphabet  The character alphabet.
 *  @param[in]  incremental         Use to specify if add new entry into the alphabet.
 */
bool read_segmentation_instance(std::istream& is, Segmentation& output,
    bool segmented = false);

/**
 *
 *
 *
 */
void write_segmentation_instance(std::ostream& os, const Segmentation& output);

} //  end for namespace io
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_UTILS_IO_INSTANCE_SEGMENTATION_H__
