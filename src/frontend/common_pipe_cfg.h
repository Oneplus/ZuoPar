#ifndef __ZUOPAR_FRONTENT_COMMON_PIPE_CFG_H__
#define __ZUOPAR_FRONTENT_COMMON_PIPE_CFG_H__

namespace ZuoPar {
namespace FrontEnd {

class CommonPipeConfigure {
public:
  //! The supported modes.
  enum PipeMode { kPipeLearn, kPipeTest };

  //!
  CommonPipeConfigure(PipeMode mode_): mode(mode_) {}

protected:
  //! Use to specify if perform training.
  PipeMode mode;

  //! The path to the reference file.
  std::string reference_path;

  //! The path to the model file.
  std::string model_path;

  //! The path to the input file.
  std::string input_path;

  //! The path to the output file.
  std::string output_path;

  //!
  bool early_update;

  //! The size of the beam.
  int beam_size;

  //! The display.
  int display_interval;
};

} //  namespace frontend
} //  namespace zuopar

#endif  //  end for __ZUOPAR_FRONTENT_COMMON_PIPE_CFG_H__
