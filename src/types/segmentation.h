#ifndef __ZUOPAR_TYPES_SEGMENTATION_H__
#define __ZUOPAR_TYPES_SEGMENTATION_H__

#include <vector>
#include "common.h"

namespace ZuoPar {

class Segmentation {
public:
  enum { kBegin, kIntermediate };
  //! The characters.
  std::vector<character_t> chars;
  //! The bitags.
  std::vector<tag_t> tags;

  std::size_t size() const { return chars.size(); }
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_TYPES_SEGMENTATION_H__
