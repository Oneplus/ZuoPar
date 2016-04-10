#ifndef __ZUOPAR_APP_DEPPARSER_UTILS_H__
#define __ZUOPAR_APP_DEPPARSER_UTILS_H__

#include <tuple>
#include <boost/regex.hpp>
#ifndef _MSC_VER
#include <boost/regex/icu.hpp>
#endif
#include "types/dependency.h"

namespace ZuoPar {
namespace DependencyParser {

class DependencyParserUtils {
public:
  enum EvaluationStrategy { kIncludePunctuation, kCoNLLx, kChen14en, kChen14ch };

  template <typename HeadListType>
  static std::tuple<int, int> evaluate_inc_punc(
      const std::vector<std::string>& forms,
      const std::vector<std::string>& postags,
      const HeadListType& predicted_heads,
      const HeadListType& heads,
      bool dummy) {
    int nr_tokens = 0, corr_heads = 0, corr_deprels = 0;

    auto L = heads.size();
    for (auto i = (dummy ? 1: 0); i < L; ++ i) { // ignore dummy root
      ++ nr_tokens;
      if (predicted_heads[i] == heads[i]) { ++ corr_heads; }
    }
    return std::make_tuple(nr_tokens, corr_heads);
  }


  template <typename HeadListType, typename DeprelListType>
  static std::tuple<int, int, int> evaluate_inc_punc(
      const std::vector<std::string>& forms,
      const std::vector<std::string>& postags,
      const HeadListType& predicted_heads,
      const DeprelListType& predicted_deprels,
      const HeadListType& heads,
      const DeprelListType& deprels,
      bool dummy) {
    int nr_tokens = 0, corr_heads = 0, corr_deprels = 0;

    auto L = heads.size();
    for (auto i = (dummy ? 1: 0); i < L; ++ i) { // ignore dummy root
      ++ nr_tokens;
      if (predicted_heads[i] == heads[i]) {
        ++ corr_heads;
        if (predicted_deprels[i] == deprels[i]) { ++ corr_deprels; }
      }
    }
    return std::make_tuple(nr_tokens, corr_heads, corr_deprels);
  }


  template <typename HeadListType>
  static std::tuple<int, int> evaluate_conllx(
      const std::vector<std::string>& forms,
      const std::vector<std::string>& postags,
      const HeadListType& predicted_heads,
      const HeadListType& heads,
      bool dummy) {
    int nr_tokens = 0, corr_heads = 0, corr_deprels = 0;

    auto L = heads.size();
    for (auto i = (dummy ? 1: 0); i < L; ++ i) { // ignore dummy root
#ifndef _MSC_VER
      if (boost::u32regex_match(forms[i], boost::make_u32regex("[[:P*:]]*"))) {
        continue;
      }
#endif
      ++ nr_tokens;
      if (predicted_heads[i] == heads[i]) { ++ corr_heads; }
    }
    return std::make_tuple(nr_tokens, corr_heads);
  }


  template <typename HeadListType, typename DeprelListType>
  static std::tuple<int, int, int> evaluate_conllx(
      const std::vector<std::string>& forms,
      const std::vector<std::string>& postags,
      const HeadListType& predicted_heads,
      const DeprelListType& predicted_deprels,
      const HeadListType& heads,
      const DeprelListType& deprels,
      bool dummy) {
    int nr_tokens = 0, corr_heads = 0, corr_deprels = 0;

    auto L = heads.size();
    for (auto i = (dummy ? 1: 0); i < L; ++ i) { // ignore dummy root
#ifndef _MSC_VER
      if (boost::u32regex_match(forms[i], boost::make_u32regex("[[:P*:]]*"))) {
        continue;
      }
#endif
      ++ nr_tokens;
      if (predicted_heads[i] == heads[i]) {
        ++ corr_heads;
        if (predicted_deprels[i] == deprels[i]) { ++ corr_deprels; }
      }
    }
    return std::make_tuple(nr_tokens, corr_heads, corr_deprels);
  }


  template <typename HeadListType>
  static std::tuple<int, int> evaluate_chen14en(
      const std::vector<std::string>& forms,
      const std::vector<std::string>& postags,
      const HeadListType& predicted_heads,
      const HeadListType& heads,
      bool dummy) {

    int nr_tokens = 0, corr_heads = 0;

    auto L = heads.size();
    for (auto i = (dummy? 1: 0); i < L; ++ i) { // ignore dummy root
      if (postags[i] == "``" || postags[i] == "''" || postags[i] == "," ||
          postags[i] == "." || postags[i] == ":") { continue; }

      ++ nr_tokens;
      if (predicted_heads[i] == heads[i]) { ++ corr_heads; }
    }
    return std::make_tuple(nr_tokens, corr_heads);
  }


  template <typename HeadListType, typename DeprelListType>
  static std::tuple<int, int, int> evaluate_chen14en(
      const std::vector<std::string>& forms,
      const std::vector<std::string>& postags,
      const HeadListType& predicted_heads,
      const DeprelListType& predicted_deprels,
      const HeadListType& heads,
      const DeprelListType& deprels,
      bool dummy) {

    int nr_tokens = 0, corr_heads = 0, corr_deprels = 0;

    auto L = heads.size();
    for (auto i = (dummy? 1: 0); i < L; ++ i) { // ignore dummy root
      if (postags[i] == "``" || postags[i] == "''" || postags[i] == "," ||
          postags[i] == "." || postags[i] == ":") { continue; }

      ++ nr_tokens;
      if (predicted_heads[i] == heads[i]) {
        ++ corr_heads;
        if (predicted_deprels[i] == deprels[i]) { ++ corr_deprels; }
      }
    }
    return std::make_tuple(nr_tokens, corr_heads, corr_deprels);
  }


  template <typename HeadListType>
  static std::tuple<int, int> evaluate_chen14ch(
      const std::vector<std::string>& forms,
      const std::vector<std::string>& postags,
      const HeadListType& predicted_heads,
      const HeadListType& heads,
      bool dummy) {

    int nr_tokens = 0, corr_heads = 0;

    auto L = heads.size();
    for (auto i = (dummy? 1: 0); i < L; ++ i) { // ignore dummy root
      if (postags[i] == "PU") { continue; }

      ++ nr_tokens;
      if (predicted_heads[i] == heads[i]) { ++ corr_heads; }
    }
    return std::make_tuple(nr_tokens, corr_heads);
  }


  template <typename HeadListType, typename DeprelListType>
  static std::tuple<int, int, int> evaluate_chen14ch(
      const std::vector<std::string>& forms,
      const std::vector<std::string>& postags,
      const HeadListType& predicted_heads,
      const DeprelListType& predicted_deprels,
      const HeadListType& heads,
      const DeprelListType& deprels,
      bool dummy) {

    int nr_tokens = 0, corr_heads = 0, corr_deprels = 0;

    auto L = heads.size();
    for (auto i = (dummy? 1: 0); i < L; ++ i) { // ignore dummy root
      if (postags[i] == "PU") { continue; }

      ++ nr_tokens;
      if (predicted_heads[i] == heads[i]) {
        ++ corr_heads;
        if (predicted_deprels[i] == deprels[i]) { ++ corr_deprels; }
      }
    }
    return std::make_tuple(nr_tokens, corr_heads, corr_deprels);
  }

  static std::tuple<int, int, int> evaluate_conllx(
      const RawCoNLLXDependency& data,
      const std::vector<int>& heads,
      const std::vector<std::string>& deprels) {
    return evaluate_conllx(data.forms, data.postags, heads, deprels,
        data.heads, data.deprels, true);
  }

  //! TODO modify this
  static std::tuple<int, int, int> evaluate_chen14en(
      const RawCoNLLXDependency& data,
      const std::vector<int>& heads,
      const std::vector<std::string>& deprels) {
    int nr_tokens = 0, corr_heads = 0, corr_deprels = 0;

    auto L = heads.size();
    for (auto i = 1; i < L; ++ i) { // ignore dummy root
      if (data.feats[i].size() > 0 && (
            data.feats[i][0] == "``" || data.feats[i][0] == "''" ||
            data.feats[i][0] == ","  || data.feats[i][0] == "." ||
            data.feats[i][0] == ":")
         ) { continue; }

      ++ nr_tokens;
      if (heads[i] == data.heads[i]) {
        ++ corr_heads;
        if (deprels[i] == data.deprels[i]) { ++ corr_deprels; }
      }
    }
    return std::make_tuple(nr_tokens, corr_heads, corr_deprels);
  }

  //! TODO modify this
  static std::tuple<int, int, int> evaluate_chen14ch(
      const RawCoNLLXDependency& data,
      const std::vector<int>& heads,
      const std::vector<std::string>& deprels) {
    int nr_tokens = 0, corr_heads = 0, corr_deprels = 0;

    auto L = heads.size();
    for (auto i = 1; i < L; ++ i) { // ignore dummy root
      if (data.feats[i].size() > 0 && data.feats[i][0] == "PU") {
        continue;
      }
      ++ nr_tokens;
      if (heads[i] == data.heads[i]) {
        ++ corr_heads;
        if (deprels[i] == data.deprels[i]) { ++ corr_deprels; }
      }
    }
    return std::make_tuple(nr_tokens, corr_heads, corr_deprels);
  }

};

} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_UTILS_H__
