#ifndef __ZUOPAR_FRONTENT_COMMON_PIPE_CFG_H__
#define __ZUOPAR_FRONTENT_COMMON_PIPE_CFG_H__

#include "types/common.h"
#include "frontend/common_opt.h"

namespace ZuoPar {
namespace FrontEnd {

class CommonPipeConfigure {
public:
  //! The supported modes.
  enum PipeMode { kPipeLearn, kPipeTest };

  //! The Common pipe constructor on training mode.
  CommonPipeConfigure(const LearnOption& opts)
    : mode(kPipeLearn) {

    this->reference_path = opts.reference_path;
    this->model_path = opts.model_path;
    this->beam_size = opts.beam_size;
    this->display_interval = opts.display_interval;
    this->early_update = opts.early_update;
    this->algorithm = LearningAlgorithm::kAveragePerceptron;
    if (opts.algorithm == "pa") {
      this->algorithm = LearningAlgorithm::kPassiveAgressive;
    }
    _INFO << "::LEARN:: mode is actived.";
    _INFO << "report: beam size = " << this->beam_size;
    _INFO << "report: early update = " << (this->early_update ? "true": "false");
    _INFO << "report: learning algorithm = " << (
        this->algorithm == LearningAlgorithm::kAveragePerceptron ?
        "averaged perceptron": "passive aggressive");
    _INFO << "report: model file is " << opts.model_path;
    _INFO << "report: reference file is " << opts.reference_path;
  }

  //! The common pipe constructor on testing mode.
  CommonPipeConfigure(const TestOption& opts)
    : mode(kPipeTest) {
    this->model_path = opts.model_path;
    this->input_path = opts.input_path;
    this->output_path = opts.output_path;
    this->beam_size = opts.beam_size;
    this->display_interval = opts.display_interval;

    _INFO << "::TEST:: mode is actived.";
    _INFO << "report: beam size = " << opts.beam_size;
    _INFO << "report: model file is " << opts.model_path;
  }

protected:
  //! Use to specify if perform training.
  PipeMode mode;

  //! Use to specify the learning algorithm
  LearningAlgorithm algorithm;

  //! The path to the reference file.
  std::string reference_path;

  //! The path to the model file.
  std::string model_path;

  //! The path to the input file.
  std::string input_path;

  //! The path to the output file.
  std::string output_path;

  //! Use to specify perform early update strategy during learning.
  bool early_update;

  //! The size of the beam.
  int beam_size;

  //! The display.
  int display_interval;
};

} //  namespace frontend
} //  namespace zuopar

#endif  //  end for __ZUOPAR_FRONTENT_COMMON_PIPE_CFG_H__
