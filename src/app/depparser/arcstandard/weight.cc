#include "app/depparser/arcstandard/weight.h"
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/score_context.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

Weight::Weight() {
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0w );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S0w, S0p );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1w );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S1w, S1p );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N0w );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N0p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( N0w, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N1w );
  ZUOPAR_FEATURE_MAP_REGIST_U1( N1p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( N1w, N1p );  // port "from single words" in Z&N (2011)
  ZUOPAR_FEATURE_MAP_REGIST_T111( S1w, S1p, S0w );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S1w, S0w, S0p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S1w, S1p, S0p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S1p, S0w, S0p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S1w, S0w );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S1p, S0p );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S0p, N0p );  //  port "from word pairs" in Z&N (2011)
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, N0p, N1p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S1p, S0p, N0p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S0ldp, S1p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S0rdp, S1p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S1ldp, S1p );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S1rdp, S1p );  //  port the "from three words" in Z&N (2011)
  ZUOPAR_FEATURE_MAP_REGIST_B11( S0w, DistS0S1 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S0p, DistS0S1 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S1w, DistS0S1 );
  ZUOPAR_FEATURE_MAP_REGIST_B11( S1p, DistS0S1 );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0w, S1w, DistS0S1 );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S1p, DistS0S1 ); //  port the "distance" in Z&N (2011)
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0w, S0la );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0w, S0ra );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0p, S0la );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0p, S0ra );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S1w, S1la );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S1w, S1ra );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S1p, S1la );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S1p, S1ra ); //  port the "valency" in Z&N (2011)
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0ldw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0ldp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0ldl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0rdw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0rdp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0rdl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1ldw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1ldp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1ldl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1rdw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1rdp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1rdl );  // port the "unigram" in Z&N (2011)
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0l2dw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0l2dp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0l2dl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0r2dw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0r2dp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S0r2dl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1l2dw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1l2dp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1l2dl );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1r2dw );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1r2dp );
  ZUOPAR_FEATURE_MAP_REGIST_U1( S1r2dl );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S0ldp, S0l2dp );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S0rdp, S0r2dp );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S1p, S1ldp, S1l2dp );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S1p, S1rdp, S1r2dp );
  ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, N0p, N1p );  //  port the "third-order" in Z&N (2011)
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0w, S0lset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0p, S0lset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0w, S0rset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S0p, S0rset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S1w, S1lset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S1p, S1lset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S1w, S1rset );
  ZUOPAR_FEATURE_MAP_REGIST_B10( S1p, S1rset ); // port the "label-set" in Z&N (2011)
  //ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S1p, S0r2dp );
  //ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S1p, S0l2dp );
  //ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S1p, S1r2dp );
  //ZUOPAR_FEATURE_MAP_REGIST_T111( S0p, S1p, S1l2dp );
}

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar