#include <iostream>
#include <fstream>
#include <algorithm>
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/sequence_instance.h"
#include "utils/io/instance/sequence_instance.h"
#include "app/seqlabeler/action_utils.h"
#include "app/seqlabeler/pipe.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/regex.hpp>

namespace ZuoPar {
namespace SequenceLabeler {

namespace eg = ZuoPar::Engine;
namespace fe = ZuoPar::FrontEnd;

Pipe::Pipe(const LearnOption& opts)
  : weight(0), decoder(0), learner(0),
  fe::CommonPipeConfigure(static_cast<const fe::LearnOption&>(opts)) {
  constrain_path = opts.constrain_path;
  _INFO << "report: constrain path: " << constrain_path;

  if (load_model(opts.model_path)) {
    _INFO << "report: model is loaded.";
  } else {
    _INFO << "report: model is not loaded.";
  }
}

Pipe::Pipe(const TestOption& opts)
  : weight(0), decoder(0), learner(0),
  fe::CommonPipeConfigure(static_cast<const fe::TestOption&>(opts)) {
  constrain_path = opts.constrain_path;
  _INFO << "report: constrain path: " << constrain_path;
  if (load_model(opts.model_path)) {
    _INFO << "report: model is loaded.";
  } else {
    _INFO << "report: model is not loaded.";
  }
}

bool
Pipe::load_model(const std::string& model_path) {
  weight = new Weight;
  std::ifstream mfs(model_path);

  if (!mfs.good()) {
    _WARN << "pipe: model doesn't exists.";
    return false;
  }

  if (!attributes_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load forms alphabet.";
    return false;
  }

  if (!tags_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load tags alphabet.";
    return false;
  }

  if (!weight->load(mfs)) {
    _WARN << "pipe: failed to load weight.";
    return false;
  }

  return true;
}

bool
Pipe::save_model(const std::string& model_path) {
  std::ofstream mfs(model_path);
  if (!mfs.good()) {
    _WARN << "pipe: failed to save model.";
    return false;
  } else {
    attributes_alphabet.save(mfs);
    tags_alphabet.save(mfs);
    weight->save(mfs);
    _INFO << "pipe: model saved to " << model_path;
  }

  return true;
}

void
Pipe::load_constrain() {
  namespace algo = boost::algorithm;

  std::size_t T = tags_alphabet.size();
  trans.resize(T);
  for (std::size_t i = 0; i < T; ++ i) {
    trans[i].resize(T, false);
  }
  for (std::size_t i = 0; i < T; ++ i) {
    trans[1][i] = true;
  }

  std::ifstream ifs(constrain_path.c_str());
  _INFO << "report: load constrain from " << constrain_path;
  if (!ifs.good()) {
    for (std::size_t i = 0; i < T; ++ i) {
      for (std::size_t j = 0; j < T; ++ j) { trans[i][j] = true; }
    }
  } else {
    std::string line;
    int nr_constraints = 0;
    while (std::getline(ifs, line)) {
      std::vector<std::string> items;
      algo::trim(line);
      if (line.size() == 0) { continue; }
      algo::split_regex(items, line, boost::regex("-->"));
      BOOST_ASSERT(2 == items.size());

      algo::trim(items[0]);
      algo::trim(items[1]);
      tag_t source = tags_alphabet.encode(items[0].c_str());
      tag_t target = tags_alphabet.encode(items[1].c_str());
      if (source > 0 && target > 0) {
        //_INFO << items[0] << " " << items[1];
        trans[source][target] = true;
        nr_constraints += 1;
      }
    }
    _INFO << "report: number of constrain is: " << nr_constraints;
  }
}

bool
Pipe::setup() {
  namespace ioutils = ZuoPar::IO;

  dataset.clear();
  if (mode == kPipeLearn) {
    std::ifstream ifs(reference_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open reference file.";
      _ERROR << "#: training halt";
      return false;
    }
    _INFO << "report: loading dataset from reference file.";
    ioutils::read_sequence_instance_dataset(ifs, dataset, attributes_alphabet,
        tags_alphabet, true);
    _INFO << "report: dataset is loaded from reference file.";
  } else {
    // not implemented.
    std::ifstream ifs(input_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open input file.";
      _ERROR << "#: testing halt";
      return false;
    }
    ioutils::read_sequence_instance_dataset(ifs, dataset, attributes_alphabet,
        tags_alphabet, false);
  }
  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
  _INFO << "report: " << attributes_alphabet.size() << " attribute(s) is detected.";
  _INFO << "report: " << tags_alphabet.size() << " tag(s) is detected.";

  return true;
}

void
Pipe::run() {
  namespace ioutils = ZuoPar::IO;
  if (!setup()) {
    return;
  }

  load_constrain();

  if (mode == kPipeLearn) {
    decoder = new Decoder(tags_alphabet.size(), trans, beam_size, false, update_strategy, weight);
    learner = new Learner(weight, this->algorithm);
  } else {
    decoder = new Decoder(tags_alphabet.size(), trans, beam_size, true, update_strategy, weight);
  }

  size_t N = dataset.size();
  std::ostream* os = (mode == kPipeLearn ? NULL: ioutils::get_ostream(output_path.c_str()));

  std::vector<int> ranks;
  for (size_t i = 0; i < N; ++ i) { ranks.push_back(i); }

  while (shuffle_times --) {
    // To avoid fake shuffling.
    std::random_shuffle(ranks.begin(), ranks.end());
  }

  for (size_t n = 0; n < N; ++ n) {
    const SequenceInstance& instance = dataset[ranks[n]];
    // calculate the oracle transition actions.
    std::vector<Action> actions;
    if (mode == kPipeLearn) {
      ActionUtils::get_oracle_actions(instance, actions);
    }

    int max_nr_actions = instance.size();
    State init_state(&instance);
    Decoder::const_decode_result_t result = decoder->decode(init_state,
        actions, max_nr_actions);

    if (mode == kPipeLearn) {
      learner->set_timestamp(n+ 1);
      learner->learn(result.first, result.second);
    } else {
      SequenceInstance output;
      build_output((*result.first), output);
      ioutils::write_sequence_instance((*os), output, attributes_alphabet, tags_alphabet);
    }

    if ((n+ 1)% display_interval == 0) {
      _INFO << "pipe: processed #" << (n+ 1) << " instances.";
    }
  }
  _INFO << "pipe: processed #" << N << " instances.";

  if (mode == kPipeLearn) {
    learner->set_timestamp(N);
    learner->flush();
    _INFO << "pipe: nr errors: " << learner->errors();

    save_model(model_path);
  }
}

void
Pipe::build_output(const State& source, SequenceInstance& output) {
  size_t len = source.ref->size();

  output.resize(len);
  for (size_t i = 0; i < len; ++ i) {
    output[i].tag = source.tags[i];
  }
}


} //  end for namespace sequencelabeler
} //  end for namespace zuopar
