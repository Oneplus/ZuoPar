#ifndef __ZUOPAR_TYPES_SEGMENTATION_H__
#define __ZUOPAR_TYPES_SEGMENTATION_H__

#include <vector>
#include <string>

namespace ZuoPar {

class Segmentation {
public:
  std::vector<std::string> chars; //! The characters.
  std::vector<std::string> words;      //! The words.
  std::size_t size() const      { return chars.size(); }
  std::size_t nr_chars() const  { return chars.size(); }
  std::size_t nr_words() const  { return words.size(); }
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_TYPES_SEGMENTATION_H__
