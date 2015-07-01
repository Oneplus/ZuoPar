#include "app/depparser/nn/extractor.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

Extractor::Extractor():
  kNilForm(-1), kNilPostag(-1), kNilDeprel(-1),
  kFormInFeaturespace(-1), kPostagInFeaturespace(-1), kDeprelInFeaturespace(-1),
  kFeatureSpaceEnd(0), initialized(false) {}

void Extractor::initialize(
    int nil_form, int n_forms,
    int nil_postag, int n_postags,
    int nil_deprel, int n_deprels) {
  if (initialized) {
    _WARN << "extractor should not be initialized twice";
    return;
  }

  kFormInFeaturespace = 0;
  kFeatureSpaceEnd = n_forms;
  kNilForm = nil_form + kFormInFeaturespace;

  kPostagInFeaturespace = kFeatureSpaceEnd;
  kFeatureSpaceEnd += n_postags;
  kNilPostag = nil_postag + kPostagInFeaturespace;

  kDeprelInFeaturespace = kFeatureSpaceEnd;
  kFeatureSpaceEnd += n_deprels;
  kNilDeprel = nil_deprel + kDeprelInFeaturespace;

  initialized = true;
}

void Extractor::display() {
  _INFO << "report: form located at: [" << kFormInFeaturespace << " ... "
    << kPostagInFeaturespace- 1 << "]";
  _INFO << "report: postags located at: [" << kPostagInFeaturespace << " ... "
    << kDeprelInFeaturespace- 1 << "]";
  _INFO << "report: deprels located at: [" << kDeprelInFeaturespace << " ... "
    << kFeatureSpaceEnd - 1 << "]";
  _INFO << "report: nil form (in f.s.) =" << kNilForm;
  _INFO << "report: nil postag (in f.s.) =" << kNilPostag;
  _INFO << "report: nil deprel (in f.s.) =" << kNilDeprel;
}

void Extractor::get_basic_feature(const Context& ctx,
    const std::vector<int>& forms,
    const std::vector<int>& postags,
    const int* deprels,
    std::vector<int>& features) {
#define FORM(id)    ((ctx.id != -1) ? (forms[ctx.id]): kNilForm)
#define POSTAG(id)  ((ctx.id != -1) ? (postags[ctx.id]+ kPostagInFeaturespace): kNilPostag)
#define DEPREL(id)  ((ctx.id != -1) ? (deprels[ctx.id]+ kDeprelInFeaturespace): kNilDeprel)
#define PUSH(feat)  do { features.push_back( feat ); } while (0);

  PUSH( FORM(S0) );   PUSH( POSTAG(S0) );
  PUSH( FORM(S1) );   PUSH( POSTAG(S1) );
  PUSH( FORM(S2) );   PUSH( POSTAG(S2) );
  PUSH( FORM(N0) );   PUSH( POSTAG(N0) );
  PUSH( FORM(N1) );   PUSH( POSTAG(N1) );
  PUSH( FORM(N2) );   PUSH( POSTAG(N2) );
  PUSH( FORM(S0L) );  PUSH( POSTAG(S0L) );  PUSH( DEPREL(S0L) );
  PUSH( FORM(S0R) );  PUSH( POSTAG(S0R) );  PUSH( DEPREL(S0R) );
  PUSH( FORM(S0L2) ); PUSH( POSTAG(S0L2) ); PUSH( DEPREL(S0L2) );
  PUSH( FORM(S0R2) ); PUSH( POSTAG(S0R2) ); PUSH( DEPREL(S0R2) );
  PUSH( FORM(S0LL) ); PUSH( POSTAG(S0LL) ); PUSH( DEPREL(S0LL) );
  PUSH( FORM(S0RR) ); PUSH( POSTAG(S0RR) ); PUSH( DEPREL(S0RR) );
  PUSH( FORM(S1L) );  PUSH( POSTAG(S1L) );  PUSH( DEPREL(S1L) );
  PUSH( FORM(S1R) );  PUSH( POSTAG(S1R) );  PUSH( DEPREL(S1R) );
  PUSH( FORM(S1L2) ); PUSH( POSTAG(S1L2) ); PUSH( DEPREL(S1L2) );
  PUSH( FORM(S1R2) ); PUSH( POSTAG(S1R2) ); PUSH( DEPREL(S1R2) );
  PUSH( FORM(S1LL) ); PUSH( POSTAG(S1LL) ); PUSH( DEPREL(S1LL) );
  PUSH( FORM(S1RR) ); PUSH( POSTAG(S1RR) ); PUSH( DEPREL(S1RR) );

#undef FORM
#undef POSTAG
#undef DEPREL
#undef PUSH
}

void Extractor::get_features(const State& s, std::vector<int>& features) {
  Context ctx;
  get_context(s, ctx);
  get_basic_feature(ctx, s.ref->forms, s.ref->postags, s.deprels, features);
}

void Extractor::get_context(const State& s, Context& ctx) {
  ctx.S0 = (s.stack.size() > 0 ? s.stack[s.stack.size() - 1]: -1);
  ctx.S1 = (s.stack.size() > 1 ? s.stack[s.stack.size() - 2]: -1);
  ctx.S2 = (s.stack.size() > 2 ? s.stack[s.stack.size() - 3]: -1);
  ctx.N0 = (s.buffer < s.ref->size()    ? s.buffer:    -1);
  ctx.N1 = (s.buffer+ 1 < s.ref->size() ? s.buffer+ 1: -1);
  ctx.N2 = (s.buffer+ 2 < s.ref->size() ? s.buffer+ 2: -1);

  ctx.S0L  = (ctx.S0 >= 0  ? s.left_most_child[ctx.S0]:  -1);
  ctx.S0R  = (ctx.S0 >= 0  ? s.right_most_child[ctx.S0]: -1);
  ctx.S0L2 = (ctx.S0 >= 0  ? s.left_2nd_most_child[ctx.S0]:  -1);
  ctx.S0R2 = (ctx.S0 >= 0  ? s.right_2nd_most_child[ctx.S0]: -1);
  ctx.S0LL = (ctx.S0L >= 0 ? s.left_most_child[ctx.S0L]:  -1);
  ctx.S0RR = (ctx.S0R >= 0 ? s.right_most_child[ctx.S0R]: -1);

  ctx.S1L  = (ctx.S1 >= 0  ? s.left_most_child[ctx.S1]:  -1);
  ctx.S1R  = (ctx.S1 >= 0  ? s.right_most_child[ctx.S1]: -1);
  ctx.S1L2 = (ctx.S1 >= 0  ? s.left_2nd_most_child[ctx.S1]:  -1);
  ctx.S1R2 = (ctx.S1 >= 0  ? s.right_2nd_most_child[ctx.S1]: -1);
  ctx.S1LL = (ctx.S1L >= 0 ? s.left_most_child[ctx.S1L]: -1);
  ctx.S1RR = (ctx.S1R >= 0 ? s.right_most_child[ctx.S1R]: -1);
}

int Extractor::n_dim() const { return kFeatureSpaceEnd; }

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
