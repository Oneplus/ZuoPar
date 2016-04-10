add_subdirectory (arceager)
add_subdirectory (arcstandard)

add_executable (hc_depparser_cstep
    main.cc
    pipe.h
    pipe.cc
    weight.h
    weight.cc
    #knowledge.cc
    score_context.h
    score_context.cc
    cstep_opt_utils.h
    cstep_opt_utils.cc
    )

target_link_libraries (hc_depparser_cstep _zuo_alphabet _zuo_frontend _zuo_io _zuo_utils ${LIBS} icuuc)

set_target_properties(hc_depparser_cstep
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/experimental/)

