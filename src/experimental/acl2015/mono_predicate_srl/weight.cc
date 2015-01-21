#include "experimental/acl2015/mono_predicate_srl/weight.h"
#include "experimental/acl2015/mono_predicate_srl/action.h"
#include "experimental/acl2015/mono_predicate_srl/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

Weight::Weight() {
  ZUOPAR_FEATURE_MAP_REGIST_U1( w_2 );
  ZUOPAR_FEATURE_MAP_REGIST_U1( w_1 );
  ZUOPAR_FEATURE_MAP_REGIST_U1( w0 );
  ZUOPAR_FEATURE_MAP_REGIST_U1( w1 );
  ZUOPAR_FEATURE_MAP_REGIST_U1( w2 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( w_2, w_1 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( w_1, w0 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( w0, w1 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( w1, w2 );
  ZUOPAR_FEATURE_MAP_REGIST_U1( w_pred );
  ZUOPAR_FEATURE_MAP_REGIST_U0( position );
  ZUOPAR_FEATURE_MAP_REGIST_U0( dist );
  ZUOPAR_FEATURE_MAP_REGIST_U0( vc_pred );
  ZUOPAR_FEATURE_MAP_REGIST_B11( w0, w_pred );
  ZUOPAR_FEATURE_MAP_REGIST_B00( vc_pred, position );
  ZUOPAR_FEATURE_MAP_REGIST_B10( w_pred, position );
  ZUOPAR_FEATURE_MAP_REGIST_B10( w0, position );
  ZUOPAR_FEATURE_MAP_REGIST_T110( w0, w_pred, position );
  ZUOPAR_FEATURE_MAP_REGIST_T100( w0, vc_pred, position );
  ZUOPAR_FEATURE_MAP_REGIST_U1( p_2 );
  ZUOPAR_FEATURE_MAP_REGIST_U1( p_1 );
  ZUOPAR_FEATURE_MAP_REGIST_U1( p0 );
  ZUOPAR_FEATURE_MAP_REGIST_U1( p1 );
  ZUOPAR_FEATURE_MAP_REGIST_U1( p2 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( p_2, p_1 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( p_1, p0 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( p0, p1 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( p1, p2 );
  ZUOPAR_FEATURE_MAP_REGIST_S0( path );
  ZUOPAR_FEATURE_MAP_REGIST_U1( p_pred );
  ZUOPAR_FEATURE_MAP_REGIST_U1( p_pred_1 );
  ZUOPAR_FEATURE_MAP_REGIST_U1( p_pred1 );
  ZUOPAR_FEATURE_MAP_REGIST_B10( p_pred, position );
  ZUOPAR_FEATURE_MAP_REGIST_B10( p0, position );
  ZUOPAR_FEATURE_MAP_REGIST_T110( p0, p_pred, position );
  ZUOPAR_FEATURE_MAP_REGIST_T100( p0, vc_pred, position);
  ZUOPAR_FEATURE_MAP_REGIST_U0( t_1 );
}

} //  namespace monopredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar
