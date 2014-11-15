#include <iostream>
#include <fstream>
#include <boost/log/trivial.hpp>
#include "utils/logging.h"
#include "utils/io/dataset.h"
#include "app/depparser/arcstandard/pipe.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

Pipe::Pipe(const LearnOption& opts)
  : learning(true) {
  _INFO << "::LEARN:: mode is actived.";
  _INFO << "report: model file is " << opts.model_path;
  _INFO << "report: reference file is " << opts.reference_path;
  _INFO << "report: beam size is " << opts.beam_size;

  this->reference_path = opts.reference_path;
  this->beam_size = opts.beam_size;
}

Pipe::Pipe(const TestOption& opts)
  : learning(false) {
  BOOST_LOG_TRIVIAL(info) << "::TEST:: mode is actived.";
}

void
Pipe::run() {
  namespace ioutils = ZuoPar::IO;

  dataset.clear();
  if (learning) {
    std::ifstream ifs(reference_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open reference file.";
      _ERROR << "#: training halt";
      return;
    }

    _INFO << "report: loading dataset from reference file.";
    ioutils::read_plain_dependency_dataset(ifs, dataset, forms_alphabet,
        postags_alphabet, deprels_alphabet);
    _INFO << "report: dataset is loaded from reference file.";
    _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
    _INFO << "report: " << forms_alphabet.size() << " forms(s) is detected.";
    _INFO << "report: " << postags_alphabet.size() << " postag(s) is detected.";
    _INFO << "report: " << deprels_alphabet.size() << " deprel(s) is detected.";
  } else {
    // not implemented.
  }

  weight = new Weight;
  decoder = new Decoder(beam_size, *weight);

  size_t N = dataset.size();
  for (size_t n = 0; n < N; ++ n) {
    //Decoder::decode_result_t result = 
  }
}

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
