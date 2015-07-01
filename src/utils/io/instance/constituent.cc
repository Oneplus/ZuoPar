#include "utils/io/instance/constituent.h"
#include "utils/io/instance/lisp.h"
#include <boost/algorithm/string/trim.hpp>

namespace ZuoPar {
namespace IO {

int _read_binarized_cfg_instance_from(const LispCell& root,
    BinarizedCFGTree& output,
    Engine::TokenAlphabet& forms_alphabet,
    Engine::TokenAlphabet& postags_alphabet,
    Engine::TokenAlphabet& constituents_alphabet,
    size_t flag) {
  if (root.size() == 2 && root.list[1].size() == 0) {
    // Token
    form_t form = ((flag & 1)?
        forms_alphabet.insert(root.list[1].val):
        forms_alphabet.encode(root.list[1].val));
    postag_t postag = ((flag & (1<<1))?
        postags_alphabet.insert(root.list[0].val):
        postags_alphabet.encode(root.list[0].val));

    BinarizedCFGTreeNode new_node(form, postag);
    output.nodes.push_back(new_node);
  } else {
    // Constituent
    label_t label = ((flag & (1<<2))?
        constituents_alphabet.insert(root.list[0].val):
        constituents_alphabet.encode(root.list[0].val));

    BinarizedCFGTreeNode new_node(label);
    if (root.size() <= 2) {
      new_node.unary = true;
      new_node.left = _read_binarized_cfg_instance_from(root.list[1],
          output, forms_alphabet, postags_alphabet, constituents_alphabet, flag);
    }
    if (root.size() <= 3) {
      new_node.unary = false;
      new_node.right = _read_binarized_cfg_instance_from(root.list[1],
          output, forms_alphabet, postags_alphabet, constituents_alphabet, flag);
    }
    output.nodes.push_back(new_node);
  }
  return output.nodes.size() - 1;
}

bool read_binarized_cfg_instance(std::istream& is,
    BinarizedCFGTree& output,
    Engine::TokenAlphabet& forms_alphabet,
    Engine::TokenAlphabet& postags_alphabet,
    Engine::TokenAlphabet& constituents_alphabet,
    size_t flag) {
  namespace algo = boost::algorithm;
  std::string line;
  std::getline(is, line);
  algo::trim(line);
  if (line.size() == 0) { return false; }

  LispCell tree = read_lisp(line);
  const LispCell* root = &tree;

  while (root->size() == 1) { root = &(root->list[0]); }
  int index = _read_binarized_cfg_instance_from((*root), output,
      forms_alphabet, postags_alphabet, constituents_alphabet, flag);
  output.root_index = index;
  return true;
}

} //  namespace io
} //  namespace zuopar
