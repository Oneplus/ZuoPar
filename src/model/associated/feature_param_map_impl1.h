#ifndef __ZUOPAR_MODEL_ASSOCIATED_FEATURE_PARAM_MAP_IMPL1_H__
#define __ZUOPAR_MODEL_ASSOCIATED_FEATURE_PARAM_MAP_IMPL1_H__

#include "model/fast_associated/feature_param_map.h"

namespace ZuoPar {

template<
  class _MetaFeatureType,
  class _ScoreContextType,
  class _ActionType
>
using FeatureParameterMapImpl1 = FeatureParameterMap<
  _MetaFeatureType,
  _ScoreContextType,
  _ActionType
>;

} //  namespace zuopar

#endif  //  end for __ZUOPAR_MODEL_ASSOCIATED_FEATURE_PARAM_MAP_IMPL1_H__
