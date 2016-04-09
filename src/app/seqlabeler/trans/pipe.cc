#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/regex.hpp>
#include "utils/misc.h"
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/sequence_instance.h"
#include "utils/io/instance/sequence_instance.h"
#include "frontend/common_pipe_utils.h"
#include "app/seqlabeler/trans/action_utils.h"
#include "app/seqlabeler/trans/pipe.h"

namespace ZuoPar {
namespace SequenceLabeler {

namespace eg = ZuoPar::Engine;
namespace fe = ZuoPar::FrontEnd;
namespace ioutils = ZuoPar::IO;

Pipe::Pipe(const boost::program_options::variables_map& vm)
  : weight(new Weight), decoder(0), learner(0), conf(vm) {
  if (vm.count("constrain")) {
    _INFO << "[RPT] constrain path: " << vm["constrain"].as<std::string>();
  }

  if (vm.count("model") && load_model(vm["model"].as<std::string>())) {
    _INFO << "[RPT] model is loaded.";
  } else {
    _INFO << "[RPT] model is not loaded.";
  }
}

bool Pipe::load_model(const std::string& model_path) {
  std::ifstream mfs(model_path);

  if (!mfs.good()) {
    _WARN << "[PIP] model doesn't exists.";
    return false;
  }

  if (!attributes_alphabet.load(mfs)) {
    _WARN << "[PIP] failed to load forms alphabet.";
    return false;
  }

  if (!tags_alphabet.load(mfs)) {
    _WARN << "[PIP] failed to load tags alphabet.";
    return false;
  }

  if (!weight->load(mfs)) {
    _WARN << "[PIP] failed to load weight.";
    return false;
  }

  return true;
}

bool Pipe::save_model(const std::string& model_path) {
  std::ofstream mfs(model_path);
  if (!mfs.good()) {
    _WARN << "[PIP] failed to save model.";
    return false;
  } else {
    attributes_alphabet.save(mfs);
    tags_alphabet.save(mfs);
    weight->save(mfs);
    _INFO << "[PIP] model saved to " << model_path;
  }

  return true;
}

bool Pipe::setup(const std::string& path, std::vector<SequenceInstance>& ds, bool insert) {
  ds.clear();
  std::ifstream ifs(path);
  if (!ifs.good()) {
    return false;
  }
  _INFO << "[PIP] alphabet is " << (insert ? "open" : "close") << ".";
  _INFO << "[RPT] loading dataset from: " << path;
  ioutils::read_sequence_instance_dataset(ifs, ds, attributes_alphabet,
    tags_alphabet, insert);
  _INFO << "[RPT] dataset is loaded from reference file.";
  _INFO << "[RPT] " << ds.size() << " instance(s) is loaded.";
  return true;
}

void Pipe::load_constrain() {
  namespace algo = boost::algorithm;

  std::size_t T = tags_alphabet.size();
  trans.resize(T);
  for (std::size_t i = 0; i < T; ++ i) {
    trans[i].resize(T, false);
  }
  for (std::size_t i = 0; i < T; ++ i) {
    trans[1][i] = true;
  }

  if (conf.count("constrain")) {
    std::ifstream ifs(conf["constrain"].as<std::string>());
    _INFO << "[RPT] load constrain from " << conf["constrain"].as<std::string>();

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
        algo::trim(items[0]); algo::trim(items[1]);
        tag_t source = tags_alphabet.encode(items[0]);
        tag_t target = tags_alphabet.encode(items[1]);
        if (source > 0 && target > 0) {
          //_INFO << items[0] << " " << items[1];
          trans[source][target] = true;
          nr_constraints += 1;
        }
      }
      _INFO << "[RPT] number of constrain is: " << nr_constraints;
    }
  } else {
    _INFO << "constrain not set.";
    for (std::size_t i = 0; i < T; ++ i) {
      for (std::size_t j = 0; j < T; ++ j) { trans[i][j] = true; }
    }
  }
}

void Pipe::learn() {
  if (!setup(conf["train"].as<std::string>(), dataset, true)) {
    return;
  }
  if (!conf.count("devel") || !setup(conf["devel"].as<std::string>(), devel_dataset, false)) {
    _WARN << "[PIP] development data is not loaded";
  }
  _INFO << "[PIP] " << attributes_alphabet.size() << " attribute(s) is detected.";
  _INFO << "[PIP] " << tags_alphabet.size() << " tag(s) is detected.";

  load_constrain();
  decoder = new Decoder(tags_alphabet.size(), trans, conf["beam"].as<unsigned>(), false,
    get_update_strategy(conf["update"].as<std::string>()), weight);
  learner = new Learner(weight, get_algorithm(conf["algorithm"].as<std::string>()));

  std::string model_path = FrontEnd::get_model_name("seqlabel", conf);
  double best_score = 0.;
  unsigned n_seen = 0, N = dataset.size();
  for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
    std::random_shuffle(dataset.begin(), dataset.end());
    for (const SequenceInstance& instance : dataset) {
      n_seen++;

      std::vector<Action> actions;
      ActionUtils::get_oracle_actions(instance, actions);

      int max_nr_actions = instance.size();
      State init_state(&instance);
      Decoder::const_decode_result_t result = decoder->decode(init_state,
        actions, max_nr_actions);

      learner->set_timestamp(n_seen);
      learner->learn(result.first, result.second);

      if (n_seen % conf["report_stops"].as<unsigned>() == 0) {
        _INFO << "[PIP] processed #" << n_seen % N << "/" << n_seen / N << " instances.";
      }
      if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
        learner->flush();
        double score = evaluate(devel_dataset);
        decoder->reset_use_avg();
        _INFO << "[PIP] evaluate at instance #" << n_seen << ", score: " << score;
        if (score > best_score) {
          _INFO << "[PIP] NEW best model is achieved, save to " << model_path;
          save_model(model_path);
          best_score = score;
        }
      }
    }
    learner->flush();
    _INFO << "[PIP] iter " << iter + 1 << " #errors: " << learner->errors();
    learner->clear_errors();
    double score = evaluate(devel_dataset);
    decoder->reset_use_avg();
    _INFO << "[PIP] evaluate at the end of iteration#" << iter + 1 << " score: " << score;
    if (score > best_score) {
      _INFO << "[PIP] NEW best model is achieved, save to " << model_path;
      save_model(model_path);
      best_score = score;
    }
  }
  _INFO << "[PIP] end of training, best development score: " << best_score;
}

double Pipe::evaluate(const std::vector<SequenceInstance>& dataset) {
  std::string output = FrontEnd::get_output_name("seqlabel", conf);
  std::ostream* os = ioutils::get_ostream(output);
  decoder->set_use_avg();
  for (const SequenceInstance& instance : dataset) {
    std::vector<Action> actions;

    int max_actions = instance.size();
    State init_state(&instance);
    Decoder::const_decode_result_t result =
      decoder->decode(init_state, actions, max_actions);

    SequenceInstance output;
    build_output((*result.first), output);
    ioutils::write_sequence_instance((*os), output, attributes_alphabet, tags_alphabet);
  }
  _INFO << "[PIP] processed #" << dataset.size() << " instances.";
  if (os == (&(std::cout))) { return 0.; }
  delete os;
  return Utility::execute_script(conf["script"].as<std::string>(), output);
}

void Pipe::test() {
  if (!setup(conf["input"].as<std::string>(), dataset, false)) {
    _ERROR << "[PIP] failed to open test data, testing halted.";
    return;
  }
  _INFO << "[PIP] " << attributes_alphabet.size() << " attribute(s) is detected.";
  _INFO << "[PIP] " << tags_alphabet.size() << " tag(s) is detected.";

  decoder = new Decoder(tags_alphabet.size(), trans, conf["beam"].as<unsigned>(), false,
    get_update_strategy(conf["strategy"].as<std::string>()), weight);

  double score = evaluate(dataset);
  _INFO << "test: score " << score;
}

void Pipe::build_output(const State& source, SequenceInstance& output) {
  size_t len = source.ref->size();

  output.resize(len);
  for (size_t i = 0; i < len; ++ i) {
    output[i].tag = source.tags[i];
  }
}

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
