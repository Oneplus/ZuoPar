#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/semchunks.h"
#include "utils/io/instance/semchunks.h"
#include "experimental/acl2015/multi_predicate_srl1/action_utils.h"
#include "experimental/acl2015/multi_predicate_srl1/pipe.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

namespace eg = ZuoPar::Engine;
namespace fe = ZuoPar::FrontEnd;

Pipe::Pipe(const LearnOption& opts)
  : weight(0), decoder(0), learner(0), fe::CommonPipeConfigure(opts) {
  predicate_tag = opts.predicate_tag;
  verb_class_path = opts.verb_class_path;
  argument_prefix = opts.argument_prefix;
  if (load_model(opts.model_path)) {
    _INFO << "report: model is loaded.";
  } else {
    _INFO << "report: model is not loaded.";
  }
}

Pipe::Pipe(const TestOption& opts)
  : weight(0), decoder(0), learner(0),
  fe::CommonPipeConfigure(static_cast<const fe::TestOption&>(opts)) {
  predicate_tag = opts.predicate_tag;
  verb_class_path = opts.verb_class_path;
  argument_prefix = opts.argument_prefix;
  if (opts.output_format == "semchunks") {
    output_format = kSemanticChunks;
  } else {
    output_format = kCoNLL2005;
  }
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

  if (!forms_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load forms alphabet.";
    return false;
  }

  if (!postags_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load postags alphabet.";
    return false;
  }

  if (!senses_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load senses alphabet.";
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
    forms_alphabet.save(mfs);
    postags_alphabet.save(mfs);
    senses_alphabet.save(mfs);
    tags_alphabet.save(mfs);
    weight->save(mfs);
    _INFO << "pipe: model saved to " << model_path;
  }
  return true;
}

bool
Pipe::load_verb_class() {
  namespace algo = boost::algorithm;

  _INFO << "report: load verb class from " << verb_class_path;
  std::ifstream ifs(verb_class_path.c_str());
  if (!ifs.good()) {
    _ERROR << "#: failed to load verb class dictionary.";
    return false;
  }

  std::string line;
  verb_classes[0] = 0;
  while (std::getline(ifs, line)) {
    std::vector<std::string> tokens;
    algo::trim(line);
    if (line.size() == 0) { continue; }

    algo::split(tokens, line, boost::is_any_of("\t "),
        boost::token_compress_on);
    BOOST_ASSERT_MSG(tokens.size() == 2, "not in \"key value\" format.");

    form_t predicate = forms_alphabet.encode(tokens[0].c_str());
    if (predicate == 0) {
      _WARN << "cache: found undefined predicate: " << tokens[0];
      continue;
    }

    std::string value = tokens[1];
    algo::trim(value);
    // Encode the verb class
    algo::split(tokens, value, boost::is_any_of("C"), boost::token_compress_on);
    int val = 0;
    for (std::size_t i = 0; i < tokens.size(); ++ i) {
      if (tokens[i].size() == 0) {
        continue;
      }
      BOOST_ASSERT_MSG(boost::lexical_cast<int>(tokens[i].c_str()) < 10, "Larger than 10!");
      val += boost::lexical_cast<int>(tokens[i].c_str());
      val *= 10;
    }

    BOOST_ASSERT_MSG(val != 0, "Found Zero verb class!");
    verb_classes[predicate] = val;
  }

  _INFO << "report: loaded " << verb_classes.size() << " verb classes";
  return true;
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
    ioutils::read_semchunks_dataset(ifs, dataset, forms_alphabet, postags_alphabet,
        senses_alphabet, tags_alphabet, predicate_tag, true);
    _INFO << "report: dataset is loaded from reference file.";
  } else {
    // not implemented.
    std::ifstream ifs(input_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open input file.";
      _ERROR << "#: testing halt";
      return false;
    }
    ioutils::read_semchunks_dataset(ifs, dataset, forms_alphabet, postags_alphabet,
        senses_alphabet, tags_alphabet, predicate_tag, true);
  }
  _INFO << "report: predicate tg is \"" << predicate_tag << "\"";
  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
  _INFO << "report: " << forms_alphabet.size() << " form(s) is detected.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) is detected.";
  _INFO << "report: " << senses_alphabet.size() << " sense(s) is detected.";
  _INFO << "report: " << tags_alphabet.size() << " tag(s) is detected.";

  if (!load_verb_class()) { return false; }
  return true;
}

void
Pipe::run() {
  namespace ioutils = ZuoPar::IO;
  if (!setup()) {
    return;
  }

  size_t N = dataset.size();
  /*int max_size = beam_size;
  for (size_t n = 0; n < N; ++ n) {
    int size = beam_size * dataset[n].nr_predicates();
    if (max_size < size) { max_size = size; }
  }

  _INFO << "pipe: the actual beam size is " << max_size/beam_size
    << "*" << beam_size << "=" << max_size;*/
  decoder = new Decoder(tags_alphabet.size(), beam_size, update_strategy, weight);

  if (mode == kPipeLearn) {
    learner = new Learner(weight, this->algorithm);
  }

  std::ostream* os = (mode == kPipeLearn ? NULL: ioutils::get_ostream(output_path.c_str()));

  std::vector<std::size_t> ranks;
  for (size_t n = 0; n < N; ++ n) { ranks.push_back(n); }
  while (shuffle_times --) { std::random_shuffle(ranks.begin(), ranks.end()); }

  size_t m = 1;
  for (size_t n = 0; n < N; ++ n) {
    _TRACE << "pipe: instance #" << n;
    const SemanticChunks& instance = dataset[ranks[n]];
    //decoder->reset_beam_size(beam_size* instance.nr_predicates());

    SemanticChunks output;
    // calculate the oracle transition actions.
    std::vector<ActionCollection> actions;
    if (mode == kPipeLearn) {
      ActionUtils::get_oracle_actions(instance, actions);
      for (size_t i = 0; i < actions.size(); ++ i) {
        _TRACE << "pipe: oracle action #" << i << " " << actions[i];
      }
    }

    int max_nr_actions = instance.size();
    Information information(instance, verb_classes);
    State init_state(instance, information);
    Decoder::const_decode_result_t result = decoder->decode(init_state,
        actions, max_nr_actions);

    if (mode == kPipeLearn) {
      learner->set_timestamp(m);
      learner->learn(result.first, result.second);
    } else {
      build_output((*result.first), output);
    }

    if ((n+ 1)% display_interval == 0) {
      _INFO << "pipe: processed #" << (n+ 1) << " instances.";
    }

    if (mode != kPipeLearn) {
      output.forms = instance.forms;
      output.postags = instance.postags;
      output.senses = instance.senses;

      if (output_format == kSemanticChunks) {
        ioutils::write_semchunks_instance((*os), output, forms_alphabet,
            postags_alphabet, senses_alphabet, tags_alphabet);
      } else {
        ioutils::write_props_instance((*os), output, forms_alphabet,
            postags_alphabet, senses_alphabet, tags_alphabet);
      }
    }
  }
  _INFO << "pipe: processed #" << N << " instances.";

  if (mode == kPipeLearn) {
    learner->set_timestamp(m);
    learner->flush();
    save_model(model_path);
  }
}

void
Pipe::build_output(const State& source, SemanticChunks& output) {
  SemanticChunks::predicate_t predicate;
  int N = source.ref->nr_predicates();
  output.predicates.resize(N);
  for (int i = 0; i < N; ++ i) { output.predicates[i].second.clear(); }

  for (const State* p = &source; p->previous; p = p->previous) {
    tag_t tag;

    for (int i = 0; i < N; ++ i) {
      const Action& act = p->last_action[i];
      BOOST_ASSERT_MSG(N == last_action.size(),
          "Number of predicate and number of meta-action unmatch!");
      if (ActionUtils::is_O(act)) {
        output.predicates[i].second.push_back(kSemanticChunkOuterTag);
      } else if (ActionUtils::is_B(act, tag)) {
        output.predicates[i].second.push_back(kSemanticChunkBeginTag+ tag);
      } else if (ActionUtils::is_I(act, tag)) {
        output.predicates[i].second.push_back(kSemanticChunkInterTag+ tag);
      }
    }
  }

  for (int i = 0; i < N; ++ i) {
    std::reverse(output.predicates[i].second.begin(), output.predicates[i].second.end());
    output.predicates[i].first = source.ref->predicates[i].first;
  }

  /*for (int i = 0; i < N; ++ i) {
    for (int j = 0; j < output.predicates[i].second.size(); ++ j) {
      _DEBUG << output.predicates[i].second[j];
    }
  }*/
}

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace sequencelabeler
} //  namespace zuopar
