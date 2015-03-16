#ifndef __ZUOPAR_UTILS_IO_DATASET_CSV_H__
#define __ZUOPAR_UTILS_IO_DATASET_CSV_H__

#include <iostream>
#include <vector>
#include "engine/token_alphabet.h"

namespace ZuoPar {
namespace IO {

/**
 *
 *  @param[in]  is                The input stream.
 *  @param[out] dataset           The output dataset.
 */
void read_csv_dataset(
    std::istream& is,
    std::vector< std::vector< std::vector<std::string> > >& dataset);

}
}

#endif  //  end for __ZUOPAR_UTILS_IO_DATASET_H__
