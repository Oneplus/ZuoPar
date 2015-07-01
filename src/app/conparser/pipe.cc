#include <fstream>
#include "app/conparser/pipe.h"
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/constituent.h"
#include "utils/io/instance/constituent.h"

namespace ZuoPar {
namespace ConstituentParser {

Pipe::Pipe(const fe::LearnOption& opts)
  : fe::CommonPipeConfigure(opts) {
}

Pipe::Pipe(const LearnOption& opts)
  : fe::CommonPipeConfigure(static_cast<const fe::LearnOption&>(opts)) {
}

Pipe::Pipe(const TestOption& opts)
  : fe::CommonPipeConfigure(static_cast<const fe::TestOption&>(opts)) {
}

Pipe::~Pipe() {
}

bool Pipe::setup() {
  namespace ioutils = ZuoPar::IO;
  dataset.clear();
  if (mode == kPipeLearn) {
    std::ifstream ifs(reference_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open reference file.";
      _ERROR << "#: training halted.";
      return false;
    }
    _INFO << "report: loading dataset from reference file.";
    ioutils::read_binarized_cfg_dataset(ifs, dataset, forms_alphabet,
        postags_alphabet, constituents_alphabet);
    _INFO << "report: dataset is loaded from reference file.";
  } else {
    std::ifstream ifs(input_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open input file.";
      _ERROR << "#: training halted.";
      return false;
    }
    ioutils::read_binarized_cfg_dataset(ifs, dataset, forms_alphabet,
        postags_alphabet, constituents_alphabet, 0x03);
  }
  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
  _INFO << "report: " << forms_alphabet.size() << " form(s) is detected.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) is detected.";
  _INFO << "report: " << constituents_alphabet.size() << " constituent type(s) is detected.";
  return true;
}

void Pipe::run() {
  if (!setup()) {
    return;
  }
}

} //  namespace constituentparser
} //  namespace zuopar
