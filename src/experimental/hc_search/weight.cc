#include "experimental/hc_search/weight.h"
#include "experimental/hc_search/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace CStep {

Weight::Weight() {
  /*Begin singular*/ /*{
    __REG_1(H, deprels);
    __REG_2(H_H, postags, deprels);
    __REG_2(H_H, forms, deprels);
    __REG_3(H_H_H, forms, postags, deprels);
    __REG_3(H_pH_H, postags, postags, deprels);
    __REG_3(H_nH_H, postags, postags, deprels);
    __REG_4(pH_H_nH_H, postags, postags, postags, deprels);
  }*/ //! End singular

  /* Begin 1st-order */ {
    __REG_2(H_M, forms, forms);
    __REG_2(H_M, forms, postags);
    __REG_2(H_M, postags, forms);
    __REG_2(H_M, postags, postags);

    __REG_3(H_M_M, postags, postags, forms);
    __REG_3(H_M_M, forms, postags, forms);
    __REG_3(H_H_M, forms, postags, postags);
    __REG_3(H_H_M, forms, postags, forms);

    // distance and dir.
    __REGN_3(H_M_Dist, forms, forms);
    __REGN_3(H_M_Dist, forms, postags);
    __REGN_3(H_M_Dist, postags, forms);
    __REGN_3(H_M_Dist, postags, postags);

    __REGN_4(H_M_M_Dist, postags, postags, forms);
    __REGN_4(H_M_M_Dist, forms, postags, forms);
    __REGN_4(H_H_M_Dist, forms, postags, postags);
    __REGN_4(H_H_M_Dist, forms, postags, forms);

    __REGN_3(H_M_Dir, forms, forms);
    __REGN_3(H_M_Dir, forms, postags);
    __REGN_3(H_M_Dir, postags, forms);
    __REGN_3(H_M_Dir, postags, postags);

    __REGN_4(H_M_M_Dir, postags, postags, forms);
    __REGN_4(H_M_M_Dir, forms, postags, forms);
    __REGN_4(H_H_M_Dir, forms, postags, postags);
    __REGN_4(H_H_M_Dir, forms, postags, forms);

    // deprels
    __REGN_3(H_M_Rel, forms, forms);
    __REGN_3(H_M_Rel, forms, postags);
    __REGN_3(H_M_Rel, postags, forms);
    __REGN_3(H_M_Rel, postags, postags);

    __REGN_4(H_M_M_Rel, postags, postags, forms);
    __REGN_4(H_M_M_Rel, forms, postags, forms);
    __REGN_4(H_H_M_Rel, forms, postags, postags);
    __REGN_4(H_H_M_Rel, forms, postags, forms);

    // Context.
    __REG_4(pH_H_M_Mn, postags, postags, postags, postags);
    __REG_3(pH_H_M, postags, postags, postags);
    __REG_3(H_M_Mn, postags, postags, postags);
    __REG_3(pH_H_Mn, postags, postags, postags);
    __REG_3(pH_M_Mn, postags, postags, postags);
    __REG_4(H_nH_pM_M, postags, postags, postags, postags);
    __REG_3(H_nH_M, postags, postags, postags);
    __REG_3(H_pM_M, postags, postags, postags);
    __REG_3(nH_pM_M, postags, postags, postags);
    __REG_3(H_nH_pM, postags, postags, postags);
  } //  end 1st-order

  /*begin 2nd-order: sibling*/ {
    __REG_3(H_S_M, postags, postags, postags);
    __REG_3(H_S_M, forms, postags, postags);
    __REG_3(H_S_M, postags, forms, postags);
    __REG_3(H_S_M, postags, postags, forms);

    __REG_2(S_M, postags, postags);
    __REG_2(S_M, forms, forms);
    __REG_2(S_M, postags, forms);
    __REG_2(S_M, forms, postags);

    // direction
    __REGN_4(H_S_M_Dir, postags, postags, postags);
    __REGN_4(H_S_M_Dir, forms, postags, postags);
    __REGN_4(H_S_M_Dir, postags, forms, postags);
    __REGN_4(H_S_M_Dir, postags, postags, forms);

    __REGN_3(S_M_Dist, postags, postags);
    __REGN_3(S_M_Dist, forms, forms);
    __REGN_3(S_M_Dist, postags, forms);
    __REGN_3(S_M_Dist, forms, postags);

    // deprels
    __REGN_4(H_S_M_Rel, postags, postags, postags);
    __REGN_4(H_S_M_Rel, forms, postags, postags);
    __REGN_4(H_S_M_Rel, postags, forms, postags);
    __REGN_4(H_S_M_Rel, postags, postags, forms);

    /*__REGN_3(S_M_Rel, postags, postags);
    __REGN_3(S_M_Rel, forms, forms);
    __REGN_3(S_M_Rel, postags, forms);
    __REGN_3(S_M_Rel, forms, postags);*/

    // - Context
    /*__REG_4(pH_H_S_M, postags, postags, postags, postags);
    __REG_4(H_nH_S_M, postags, postags, postags, postags);
    __REG_4(H_pS_S_M, postags, postags, postags, postags);
    __REG_4(H_S_nS_M, postags, postags, postags, postags);
    __REG_4(H_S_pM_M, postags, postags, postags, postags);
    __REG_4(H_S_M_nM, postags, postags, postags, postags);

    __REG_4(pH_H_S_M, postags, forms, postags, postags);
    __REG_4(H_nH_S_M, forms, postags, postags, postags);
    __REG_4(H_pS_S_M, forms, postags, postags, postags);
    __REG_4(H_S_nS_M, forms, postags, postags, postags);
    __REG_4(H_S_pM_M, forms, postags, postags, postags);
    __REG_4(H_S_M_nM, forms, postags, postags, postags);

    __REG_4(pH_H_S_M, postags, postags, forms, postags);
    __REG_4(H_nH_S_M, postags, postags, forms, postags);
    __REG_4(H_pS_S_M, postags, postags, forms, postags);
    __REG_4(H_S_nS_M, postags, forms, postags, postags);
    __REG_4(H_S_pM_M, postags, forms, postags, postags);
    __REG_4(H_S_M_nM, postags, forms, postags, postags);

    __REG_4(pH_H_S_M, postags, postags, postags, forms);
    __REG_4(H_nH_S_M, postags, postags, postags, forms);
    __REG_4(H_pS_S_M, postags, postags, postags, forms);
    __REG_4(H_S_nS_M, postags, postags, postags, forms);
    __REG_4(H_S_pM_M, postags, postags, postags, forms);
    __REG_4(H_S_M_nM, postags, postags, forms, postags);*/
  } // end for 2nd-order sibling

  /*begin 2nd-order: grandparent*/ {
    __REG_3(G_P_M, postags, postags, postags);
    __REG_3(G_P_M, forms, postags, postags);
    __REG_3(G_P_M, postags, forms, postags);
    __REG_3(G_P_M, postags, postags, forms);

    __REG_2(G_M, postags, postags);
    __REG_2(G_M, forms, postags);
    __REG_2(G_M, postags, forms);

    // direction
    __REGN_4(G_P_M_Dir, postags, postags, postags);
    __REGN_4(G_P_M_Dir, forms, postags, postags);
    __REGN_4(G_P_M_Dir, postags, forms, postags);
    __REGN_4(G_P_M_Dir, postags, postags, forms);

    __REGN_3(G_M_Dir, postags, postags);
    __REGN_3(G_M_Dir, forms, postags);
    __REGN_3(G_M_Dir, postags, forms);
    __REGN_3(G_M_Dir, forms, forms);

    // deprels
    __REGN_4(G_P_M_Rel, postags, postags, postags);
    __REGN_4(G_P_M_Rel, forms, postags, postags);
    __REGN_4(G_P_M_Rel, postags, forms, postags);
    __REGN_4(G_P_M_Rel, postags, postags, forms);

    // __REGN_3(G_M_Rel, postags, postags);
    // __REGN_3(G_M_Rel, forms, postags);
    // __REGN_3(G_M_Rel, postags, forms);

    // context
    /*__REG_4(pG_G_P_M, postags, postags, postags, postags);
    __REG_4(G_nG_P_M, postags, postags, postags, postags);
    __REG_4(G_pP_P_M, postags, postags, postags, postags);
    __REG_4(G_P_nP_M, postags, postags, postags, postags);
    __REG_4(G_P_pM_M, postags, postags, postags, postags);
    __REG_4(G_P_M_nM, postags, postags, postags, postags);

    __REG_4(pG_G_P_M, postags, forms, postags, postags);
    __REG_4(G_nG_P_M, forms, postags, postags, postags);
    __REG_4(G_pP_P_M, forms, postags, postags, postags);
    __REG_4(G_P_nP_M, forms, postags, postags, postags);
    __REG_4(G_P_pM_M, forms, postags, postags, postags);
    __REG_4(G_P_M_nM, forms, postags, postags, postags);

    __REG_4(pG_G_P_M, postags, postags, forms, postags);
    __REG_4(G_nG_P_M, postags, postags, forms, postags);
    __REG_4(G_pP_P_M, postags, postags, forms, postags);
    __REG_4(G_P_nP_M, postags, forms, postags, postags);
    __REG_4(G_P_pM_M, postags, forms, postags, postags);
    __REG_4(G_P_M_nM, postags, forms, postags, postags);

    __REG_4(pG_G_P_M, postags, postags, postags, forms);
    __REG_4(G_nG_P_M, postags, postags, postags, forms);
    __REG_4(G_pP_P_M, postags, postags, postags, forms);
    __REG_4(G_P_nP_M, postags, postags, postags, forms);
    __REG_4(G_P_pM_M, postags, postags, postags, forms);
    __REG_4(G_P_M_nM, postags, postags, forms, postags);*/

    __REG_4(H1_H_M1_M, postags, postags, postags, postags);
    __REGN_5(H1_H_M1_M_Dir, postags, postags, postags, postags);
  } // end for 2nd-order grandparent

  /* Third-order */ {
    __REG_4(G_P_M_C, postags, postags, postags, postags);
    __REG_4(G_P_M_C, forms, postags, postags, postags);
    __REG_4(G_P_M_C, postags, forms, postags, postags);
    __REG_4(G_P_M_C, postags, postags, forms, postags);
    __REG_4(G_P_M_C, postags, postags, postags, forms);

    __REGN_5(G_P_M_C_Dir, postags, postags, postags, postags);
    __REGN_5(G_P_M_C_Dir, forms, postags, postags, postags);
    __REGN_5(G_P_M_C_Dir, postags, forms, postags, postags);
    __REGN_5(G_P_M_C_Dir, postags, postags, forms, postags);
    __REGN_5(G_P_M_C_Dir, postags, postags, postags, forms);

    __REGN_5(G_P_M_C_Rel, postags, postags, postags, postags);
    __REGN_5(G_P_M_C_Rel, forms, postags, postags, postags);
    __REGN_5(G_P_M_C_Rel, postags, forms, postags, postags);
    __REGN_5(G_P_M_C_Rel, postags, postags, forms, postags);
    __REGN_5(G_P_M_C_Rel, postags, postags, postags, forms);

    __REG_3(G_P_C, postags, postags, postags);
    __REG_3(G_P_C, forms, postags, postags);
    __REG_3(G_P_C, postags, forms, postags);
    __REG_3(G_P_C, postags, postags, forms);

    __REGN_4(G_P_C_Dir, postags, postags, postags);
    __REGN_4(G_P_C_Dir, forms, postags, postags);
    __REGN_4(G_P_C_Dir, postags, forms, postags);
    __REGN_4(G_P_C_Dir, postags, postags, forms);

    //__REGN_4(G_P_C_Rel, postags, postags, postags);
    //__REGN_4(G_P_C_Rel, forms, postags, postags);
    //__REGN_4(G_P_C_Rel, postags, forms, postags);
    //__REGN_4(G_P_C_Rel, postags, postags, forms);

    __REG_3(G_M_C, postags, postags, postags);
    __REG_3(G_M_C, forms, postags, postags);
    __REG_3(G_M_C, postags, forms, postags);
    __REG_3(G_M_C, postags, postags, forms);

    __REGN_4(G_M_C_Dir, postags, postags, postags);
    __REGN_4(G_M_C_Dir, forms, postags, postags);
    __REGN_4(G_M_C_Dir, postags, forms, postags);
    __REGN_4(G_M_C_Dir, postags, postags, forms);

    //__REGN_4(G_M_C_Rel, postags, postags, postags);
    //__REGN_4(G_M_C_Rel, forms, postags, postags);
    //__REGN_4(G_M_C_Rel, postags, forms, postags);
    //__REGN_4(G_M_C_Rel, postags, postags, forms);

    __REG_2(G_C, postags, postags);
    __REG_2(G_C, forms, postags);
    __REG_2(G_C, postags, forms);

    __REGN_3(G_C_Dir, postags, postags);
    __REGN_3(G_C_Dir, forms, postags);
    __REGN_3(G_C_Dir, postags, forms);

    //__REGN_3(G_C_Rel, postags, postags);
    //__REGN_3(G_C_Rel, forms, postags);
    //__REGN_3(G_C_Rel, postags, forms);

    __REG_4(G_P_M_S, postags, postags, postags, postags);
    __REG_4(G_P_M_S, forms, postags, postags, postags);
    __REG_4(G_P_M_S, postags, forms, postags, postags);
    __REG_4(G_P_M_S, postags, postags, forms, postags);
    __REG_4(G_P_M_S, postags, postags, postags, forms);

    __REGN_5(G_P_M_S_Dir, postags, postags, postags, postags);
    __REGN_5(G_P_M_S_Dir, forms, postags, postags, postags);
    __REGN_5(G_P_M_S_Dir, postags, forms, postags, postags);
    __REGN_5(G_P_M_S_Dir, postags, postags, forms, postags);
    __REGN_5(G_P_M_S_Dir, postags, postags, postags, forms);

    __REGN_5(G_P_M_S_Rel, postags, postags, postags, postags);
    __REGN_5(G_P_M_S_Rel, forms, postags, postags, postags);
    __REGN_5(G_P_M_S_Rel, postags, forms, postags, postags);
    __REGN_5(G_P_M_S_Rel, postags, postags, forms, postags);
    __REGN_5(G_P_M_S_Rel, postags, postags, postags, forms);

    __REG_3(G_M_S, postags, postags, postags);
    __REG_3(G_M_S, forms, postags, postags);
    __REG_3(G_M_S, postags, forms, postags);
    __REG_3(G_M_S, postags, postags, forms);

    __REGN_4(G_M_S_Dir, postags, postags, postags);
    __REGN_4(G_M_S_Dir, forms, postags, postags);
    __REGN_4(G_M_S_Dir, postags, forms, postags);
    __REGN_4(G_M_S_Dir, postags, postags, forms);

    //__REGN_4(G_M_S_Rel, postags, postags, postags);
    //__REGN_4(G_M_S_Rel, forms, postags, postags);
    //__REGN_4(G_M_S_Rel, postags, forms, postags);
    //__REGN_4(G_M_S_Rel, postags, postags, forms);

    __REG_4(H_M_S_T, postags, postags, postags, postags);
    __REG_4(H_M_S_T, forms, postags, postags, postags);
    __REG_4(H_M_S_T, postags, forms, postags, postags);
    __REG_4(H_M_S_T, postags, postags, forms, postags);
    __REG_4(H_M_S_T, postags, postags, postags, forms);

    __REGN_5(H_M_S_T_Dir, postags, postags, postags, postags);
    __REGN_5(H_M_S_T_Dir, forms, postags, postags, postags);
    __REGN_5(H_M_S_T_Dir, postags, forms, postags, postags);
    __REGN_5(H_M_S_T_Dir, postags, postags, forms, postags);
    __REGN_5(H_M_S_T_Dir, postags, postags, postags, forms);

    //__REGN_5(H_M_S_T_Rel, postags, postags, postags, postags);
    //__REGN_5(H_M_S_T_Rel, forms, postags, postags, postags);
    //__REGN_5(H_M_S_T_Rel, postags, forms, postags, postags);
    //__REGN_5(H_M_S_T_Rel, postags, postags, forms, postags);
    //__REGN_5(H_M_S_T_Rel, postags, postags, postags, forms);

    __REG_3(H_M_T, postags, postags, postags);
    __REG_3(H_M_T, forms, postags, postags);
    __REG_3(H_M_T, postags, forms, postags);
    __REG_3(H_M_T, postags, postags, forms);

    __REGN_4(H_M_T_Dir, postags, postags, postags);
    __REGN_4(H_M_T_Dir, forms, postags, postags);
    __REGN_4(H_M_T_Dir, postags, forms, postags);
    __REGN_4(H_M_T_Dir, postags, postags, forms);

    __REG_3(M_S_T, postags, postags, postags);
    __REG_3(M_S_T, forms, postags, postags);
    __REG_3(M_S_T, postags, forms, postags);
    __REG_3(M_S_T, postags, postags, forms);

    __REGN_4(M_S_T_Dir, postags, postags, postags);
    __REGN_4(M_S_T_Dir, forms, postags, postags);
    __REGN_4(M_S_T_Dir, postags, forms, postags);
    __REGN_4(M_S_T_Dir, postags, postags, forms);

    //__REGN_4(H_M_T_Rel, postags, postags, postags);
    //__REGN_4(H_M_T_Rel, forms, postags, postags);
    //__REGN_4(H_M_T_Rel, postags, forms, postags);
    //__REGN_4(H_M_T_Rel, postags, postags, forms);

    //! outer sibling grandchild: postags
    __REG_4(H_S_M_GC, postags, postags, postags, postags);
    __REG_4(H_S_M_GC, forms, postags, postags, postags);
    __REG_4(H_S_M_GC, postags, forms, postags, postags);
    __REG_4(H_S_M_GC, postags, postags, forms, postags);
    __REG_4(H_S_M_GC, postags, postags, postags, forms);

    __REGN_5(H_S_M_GC_Dir, postags, postags, postags, postags);
    __REGN_5(H_S_M_GC_Dir, forms, postags, postags, postags);
    __REGN_5(H_S_M_GC_Dir, postags, forms, postags, postags);
    __REGN_5(H_S_M_GC_Dir, postags, postags, forms, postags);
    __REGN_5(H_S_M_GC_Dir, postags, postags, postags, forms);

    //__REGN_5(H_S_M_GC_Rel, postags, postags, postags, postags);
    //__REGN_5(H_S_M_GC_Rel, forms, postags, postags, postags);
    //__REGN_5(H_S_M_GC_Rel, postags, forms, postags, postags);
    //__REGN_5(H_S_M_GC_Rel, postags, postags, forms, postags);
    //__REGN_5(H_S_M_GC_Rel, postags, postags, postags, forms);

    //! outer sibling grandchild: postags
    __REG_4(H_S_GC_M, postags, postags, postags, postags);
    __REG_4(H_S_GC_M, forms, postags, postags, postags);
    __REG_4(H_S_GC_M, postags, forms, postags, postags);
    __REG_4(H_S_GC_M, postags, postags, forms, postags);
    __REG_4(H_S_GC_M, postags, postags, postags, forms);

    __REGN_5(H_S_GC_M_Dir, postags, postags, postags, postags);
    __REGN_5(H_S_GC_M_Dir, forms, postags, postags, postags);
    __REGN_5(H_S_GC_M_Dir, postags, forms, postags, postags);
    __REGN_5(H_S_GC_M_Dir, postags, postags, forms, postags);
    __REGN_5(H_S_GC_M_Dir, postags, postags, postags, forms);

    //__REGN_5(H_S_GC_M_Rel, postags, postags, postags, postags);
    //__REGN_5(H_S_GC_M_Rel, forms, postags, postags, postags);
    //__REGN_5(H_S_GC_M_Rel, postags, forms, postags, postags);
    //__REGN_5(H_S_GC_M_Rel, postags, postags, forms, postags);
    //__REGN_5(H_S_GC_M_Rel, postags, postags, postags, forms);
  } // end for third-order

  /*High order*/ {
    // right -branch
    /*ufeat_map_repo.push_back(uf_map_t(
          [](const ScoreContext& ctx, std::vector<uf_t>& cache) -> void {
            cache.push_back(uf_t(ctx.RB));
          }));*/

    // span length
    __REG_2(H_H, postags, span_length);
    __REG_2(H_H, forms, span_length);

    // valency
    __REG_2(H_H, postags, nr_children);
    __REG_2(H_H, postags, nr_left_children);
    __REG_2(H_H, postags, nr_right_children);
    __REG_2(H_H, forms, nr_children);
    __REG_2(H_H, forms, nr_left_children);
    __REG_2(H_H, forms, nr_right_children);

    // label set
    __REG_2(H_H, postags, label_set);
    __REG_2(H_H, postags, left_label_set);
    __REG_2(H_H, postags, right_label_set);
    __REG_2(H_H, forms, label_set);
    __REG_2(H_H, forms, left_label_set);
    __REG_2(H_H, forms, right_label_set);

    // PP attachment
    __REG_2(PP_H_M, postags, postags);
    __REG_2(PP_H_M, forms, postags);
    __REG_2(PP_H_M, postags, forms);
    __REG_2(PP_H_M, forms, forms);

    __REG_3(PP_P_H_M, forms, postags, postags);
    __REG_3(PP_P_H_M, forms, forms, postags);
    __REG_3(PP_P_H_M, forms, postags, forms);
    __REG_3(PP_P_H_M, forms, forms, forms);

    __REGN_3(PP_H_M_Dir, postags, postags);
    __REGN_3(PP_H_M_Dir, forms, postags);
    __REGN_3(PP_H_M_Dir, postags, forms);
    __REGN_3(PP_H_M_Dir, forms, forms);

    __REGN_4(PP_P_H_M_Dir, forms, postags, postags);
    __REGN_4(PP_P_H_M_Dir, forms, forms, postags);
    __REGN_4(PP_P_H_M_Dir, forms, postags, forms);
    __REGN_4(PP_P_H_M_Dir, forms, forms, forms);
  }
}

} //  namespace cstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
