#ifndef __ZUOPAR_TYPES_SEQUENCE_INSTANCE_H__
#define __ZUOPAR_TYPES_SEQUENCE_INSTANCE_H__

#include <vector>
#include "common.h"

namespace ZuoPar {

struct SequenceItem {
  tag_t tag;
  std::vector<attribute_t> attributes;
};

typedef std::vector<SequenceItem> SequenceInstance;

} //  end for namespace zuopar


#endif  //  end for __ZUOPAR_TYPES_SEQUENCE_INSTANCE_H__
