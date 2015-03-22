#include "utils/io/instance/dependency.h"
#include "utils/io/instance/csv.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

namespace ZuoPar {
namespace IO {

void read_dependency_instance(std::istream& is,
    Dependency& output,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& deprels_alphabet,
    size_t flag) {
  std::vector< std::vector<std::string> > mat;
  read_csv(is, mat);
  for (const std::vector<std::string>& items: mat) {
    form_t form = ((flag & 1)?
        forms_alphabet.insert(items[0]) :
        forms_alphabet.encode(items[0]));

    postag_t postag = ((flag & (1<<1))?
        postags_alphabet.insert(items[1]) :
        postags_alphabet.encode(items[1]));

    deprel_t deprel = ((flag & (1<<2))?
        deprels_alphabet.insert(items[3]) :
        deprels_alphabet.encode(items[3]));
    output.push_back(form, postag, atoi(items[2].c_str()), deprel);
  }
}

void write_dependency_instance(std::ostream& os,
    const Dependency& output,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& deprels_alphabet) {
  size_t sz = output.size();
  for (auto i = 0; i < sz; ++ i) {
    os << forms_alphabet.decode(output.forms[i])  << "\t"
      << postags_alphabet.decode(output.postags[i]) << "\t"
      << output.heads[i]  << "\t"
      << deprels_alphabet.decode(output.deprels[i])
      << std::endl;
  }
  os << std::endl;
}

void read_conllx_dependency_instance(std::istream& is,
    CoNLLXDependency& output,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& lemmas_alphabet,
    eg::TokenAlphabet& cpostag_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& feat_alphabet,
    eg::TokenAlphabet& deprels_alphabet,
    size_t flag) {
  std::vector< std::vector<std::string> > mat;
  read_csv(is, mat);
  for (const std::vector<std::string>& items: mat) {
    form_t form = ((flag & 1)?
        forms_alphabet.insert(items[1]):
        forms_alphabet.encode(items[1]));

    lemma_t lemma = ((flag & (1<<1))?
        lemmas_alphabet.insert(items[2]): 
        lemmas_alphabet.encode(items[2]));

    postag_t cpostag = ((flag & (1<<2)) ?
        cpostag_alphabet.insert(items[3]):
        cpostag_alphabet.encode(items[3]));

    postag_t postag = ((flag & (1<<3))?
        postags_alphabet.insert(items[4]):
        postags_alphabet.encode(items[4]));

    feat_list_t feat_list;
    if (items[5] != "_") {
      std::vector<std::string> tokens;
      boost::algorithm::split(tokens, items[5], boost::is_any_of("|"),
          boost::token_compress_on);
      for (const std::string& token: tokens) {
        feat_list.push_back(((flag & (1<<4))?
            feat_alphabet.insert(token):
            feat_alphabet.encode(token)));
      }
    }

    deprel_t deprel = 0;
    int head = -1;

    if (items.size() > 6) {  //  for the blind test mode.
      deprel = ((flag & (1<<5))?
          deprels_alphabet.insert(items[7]):
          deprels_alphabet.encode(items[7]));
      head = atoi(items[6].c_str());
    }

    output.push_back(form, lemma, cpostag, postag, feat_list, head, deprel);
  }
}

void write_conllx_dependency_instance(std::ostream& os,
    const CoNLLXDependency& output,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& lemmas_alphabet,
    const eg::TokenAlphabet& cpostags_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& feat_alphabet,
    const eg::TokenAlphabet& deprels_alphabet) {
  size_t sz = output.size();
  for (auto i = 1; i < sz; ++ i) {  //  Count from the real word.
    os << i                                           << "\t"
      << forms_alphabet.decode(output.forms[i])       << "\t"
      << lemmas_alphabet.decode(output.lemmas[i])     << "\t"
      << cpostags_alphabet.decode(output.cpostags[i]) << "\t"
      << postags_alphabet.decode(output.postags[i])   << "\t";
    if (output.feats[i].size() == 0) {
      os << "_\t";
    } else {
      std::string rep;
      for (auto j = 0; j < output.feats[i].size(); ++ j) {
        if (j > 0) { rep += "|"; }
        rep += feat_alphabet.decode(output.feats[i][j]);
      }
      os << rep << "\t";
    }
    os << output.heads[i] << "\t"
      << deprels_alphabet.decode(output.deprels[i])
      << std::endl;
  }
  os << std::endl;
}

} //  end for namespace IO
} //  end for namespace zuopar
