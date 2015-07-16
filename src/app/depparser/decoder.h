#ifndef __ZUOPAR_APP_DEPPARSER_DECODER_H__
#define __ZUOPAR_APP_DEPPARSER_DECODER_H__

namespace ZuoPar {
namespace DependencyParser {

class BasicDecoder {
public:
  enum RootPosition {
    kNone,
    kLeft,
    kRight
  };

  BasicDecoder(int n, int deprel, RootPosition position):
    n_deprels(n), root_deprel(deprel), root_position(position) {}

protected:
  int n_deprels;    //! The number of dependency relation
  int root_deprel;  //! The root dependency relation
  RootPosition root_position;
};

} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_DECODER_H__
