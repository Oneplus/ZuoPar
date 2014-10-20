#ifndef __ZUOPAR_TYPES_INSTANCE_H__
#define __ZUOPAR_TYPES_INSTANCE_H__

#include <iostream>
#include <vector>
#include <string>
#include "engine/alphabet.h"

namespace ZuoPar {

// Type define
typedef int word_t;
typedef int postag_t;
typedef int deprel_t;
typedef std::vector<word_t> sentence_t;

//! The struct for dependency parse
struct DependencyParse {
  DependencyParse(const Alphabet& word_alphabet,
      const Alphabet& postag_alphabet,
      const Alphabet& deprel_alphabet);

  //! The forms.
  std::vector<word_t>     forms;
  //! The postags.
  std::vector<postag_t>   postags;
  //! The heads
  std::vector<int>        heads;
  //! The dependency relation.
  std::vector<deprel_t>   deprels;
  //! The field for storing extra information.
  std::string             extras;

  //! Clear up.
  void clear();
  //! Push a form and 
  void push_back(const word_t & form,
      const postag_t & postag,
      const int head,
      const deprel_t & deprel);

  void push_back(const word_t& form,
      const postag_t& postag,
      const int head,
      const deprel_t& deprel,
      const std::string& extra);

  size_t size() const;

  friend std::ostream& operator << (std::ostream& os, const DependencyParse& d);
};

struct DependencyParseWithNP: public DependencyParse {
  typedef std::pair<int, int> range_t;

  //! Used to specify if the certain token is phrases.
  std::vector<bool> is_phrases;
  //!
  std::vector<range_t> phrases;
  //!
  std::vector<word_t> words;

  void push_back(const word_t & form,
      const postag_t & postag,
      const int head,
      const deprel_t & deprel,
      const std::vector<word_t> & extended_words,
      const range_t & phrase,
      bool is_phrase);

  void push_back(const word_t& form,
      const postag_t& postag,
      const int head,
      const deprel_t& deprel,
      const std::vector<word_t>& extended_words,
      const range_t& phrase,
      bool is_phrase,
      const std::string& extra);

  friend std::ostream& operator << (std::ostream& os, const DependencyParseWithNP& d);
};

typedef DependencyParse dependency_t;
typedef DependencyParse unordered_dependency_t;

}

#endif  //  end for __ZUOPAR_TYPES_INSTANCE_H__
