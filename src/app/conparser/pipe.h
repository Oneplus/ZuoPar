#ifndef __ZUOPAR_APP_CONPARSER_PIPE_H__
#define __ZUOPAR_APP_CONPARSER_PIPE_H__

#include "types/constituent.h"
#include "engine/token_alphabet.h"
#include "frontend/common_pipe_cfg.h"
#include "app/conparser/opt.h"

namespace ZuoPar {
namespace ConstituentParser {

class Pipe: public fe::CommonPipeConfigure {
protected:
  Pipe(const fe::LearnOption& opts);
public:
  Pipe(const LearnOption& opts);

  Pipe(const TestOption& opts);

  ~Pipe();

  bool setup();

  void run();
private:
  Engine::TokenAlphabet forms_alphabet;         //! The alphabet of forms
  Engine::TokenAlphabet postags_alphabet;       //! The alphabet of postags
  Engine::TokenAlphabet constituents_alphabet;  //! The alphabet of constituents
  std::vector<BinarizedCFGTree> dataset;        //! The dataset.
};

} //  namespace constituentparser
} //  namespace zuopar


#endif  //  end for __ZUOPAR_APP_CONPARSER_PIPE_H__
