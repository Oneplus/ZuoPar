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
#include "experimental/acl2015/mono_predicate_srl/action_utils.h"
#include "experimental/acl2015/mono_predicate_srl/pipe.h"
#include "experimental/acl2015/mono_predicate_srl/argument_relation_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

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

void
Pipe::collect_argument_relations() {
  std::string argument;
  argument = argument_prefix + boost::lexical_cast<std::string>(0);
  ArgumentRelationUtils::arg0 = tags_alphabet.encode(argument.c_str());
  if (ArgumentRelationUtils::arg0 == 0) {
    _WARN << "core argument 0: " << argument << " not found!";
  }

  argument = argument_prefix + boost::lexical_cast<std::string>(1);
  ArgumentRelationUtils::arg1 = tags_alphabet.encode(argument.c_str());
  if (ArgumentRelationUtils::arg1 == 0) {
    _WARN << "core argument 1: " << argument << " not found!";
  }

  argument = argument_prefix + boost::lexical_cast<std::string>(2);
  ArgumentRelationUtils::arg2 = tags_alphabet.encode(argument.c_str());
  if (ArgumentRelationUtils::arg2 == 0) {
    _WARN << "core argument 2: " << argument << " not found!";
  }

  argument = argument_prefix + boost::lexical_cast<std::string>(3);
  ArgumentRelationUtils::arg3 = tags_alphabet.encode(argument.c_str());
  if (ArgumentRelationUtils::arg3 == 0) {
    _WARN << "core argument 3: " << argument << " not found!";
  }

  argument = argument_prefix + boost::lexical_cast<std::string>(4);
  ArgumentRelationUtils::arg4 = tags_alphabet.encode(argument.c_str());
  if (ArgumentRelationUtils::arg4 == 0) {
    _WARN << "core argument 4: " << argument << " not found!";
  }
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
    //_DEBUG << value;
    algo::trim(value);
    // Encode the verb class
    algo::split(tokens, value, boost::is_any_of("C"), boost::token_compress_on);
    int val = 0;
    for (std::size_t i = 0; i < tokens.size(); ++ i) {
      if (tokens[i].size() == 0) {
        continue;
      }
      //_DEBUG << tokens[i];
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
  _INFO << "report: predicate tag is \"" << predicate_tag << "\"";
  _INFO << "report: argument prefix is \"" << argument_prefix << "\"";
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

  collect_argument_relations();

  if (mode == kPipeLearn) {
    decoder = new Decoder(tags_alphabet.size(), beam_size, false, update_strategy, weight);
    learner = new Learner(weight, this->algorithm);
  } else {
    decoder = new Decoder(tags_alphabet.size(), beam_size, true, update_strategy, weight);
  }
  size_t N = dataset.size();
  size_t m = 1;
  std::ostream* os = (mode == kPipeLearn ? NULL: ioutils::get_ostream(output_path.c_str()));

  std::vector<std::size_t> ranks;
  for (size_t n = 0; n < N; ++ n) { ranks.push_back(n); }
  while (shuffle_times --) { std::random_shuffle(ranks.begin(), ranks.end()); }

  for (size_t n = 0; n < N; ++ n) {
    const SemanticChunks& instance = dataset[ranks[n]];
    SemanticChunks output;
    output.forms = instance.forms;
    output.postags = instance.postags;
    output.senses = instance.senses;

    for (size_t i = 0; i < instance.nr_predicates(); ++ i, ++ m) {
      MonoSemanticChunks mono_semchunks(instance.forms,
          instance.postags,
          instance.senses,
          instance.predicates[i]);

      Information info(mono_semchunks, verb_classes);

      // calculate the oracle transition actions.
      std::vector<Action> actions;
      if (mode == kPipeLearn) {
        ActionUtils::get_oracle_actions(mono_semchunks, actions);
      }

      int max_nr_actions = instance.size();
      State init_state(&mono_semchunks, &info);
      Decoder::const_decode_result_t result = decoder->decode(init_state,
          actions, max_nr_actions);

      if (mode == kPipeLearn) {
        learner->set_timestamp(m);
        learner->learn(result.first, result.second);
      } else {
        build_output((*result.first), output);
      }
    }

    if ((n+ 1)% display_interval == 0) {
      _INFO << "pipe: processed #" << (n+ 1) << " instances.";
    }

    if (mode != kPipeLearn) {
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
    _INFO << "pipe: nr errors: " << learner->errors();
    save_model(model_path);
  }
}

void
Pipe::build_output(const State& source, SemanticChunks& output) {
  SemanticChunks::predicate_t predicate;
  predicate.first = source.ref->predicate.first;
  for (const State* p = &source; p->previous; p = p->previous) {
    tag_t tag;

    if (ActionUtils::is_O(p->last_action)) {
      predicate.second.push_back(kSemanticChunkOuterTag);
    } else if (ActionUtils::is_B(p->last_action, tag)) {
      predicate.second.push_back(kSemanticChunkBeginTag+ tag);
    } else if (ActionUtils::is_I(p->last_action, tag)) {
      predicate.second.push_back(kSemanticChunkInterTag+ tag);
    }

  }
  std::reverse(predicate.second.begin(), predicate.second.end());
  output.predicates.push_back(predicate);
}

} //  namespace monopredicatesrl
} //  namespace acl2015
} //  namespace sequencelabeler
} //  namespace zuopar
