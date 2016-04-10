add_executable (hc_depparser_hstep_arcstandard
    main.cc
    pipe.h
    pipe.cc
    ${SOURCE_DIR}/experimental/hc_search/hstep_opt_utils.h
    ${SOURCE_DIR}/experimental/hc_search/hstep_opt_utils.cc
    ${SOURCE_DIR}/app/depparser/opt_utils.h
    ${SOURCE_DIR}/app/depparser/opt_utils.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/action.h
    ${SOURCE_DIR}/app/depparser/arcstandard/action.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/action_utils.h
    ${SOURCE_DIR}/app/depparser/arcstandard/action_utils.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/state.h
    ${SOURCE_DIR}/app/depparser/arcstandard/state.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/decoder.h
    ${SOURCE_DIR}/app/depparser/arcstandard/decoder.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/weight.h
    ${SOURCE_DIR}/app/depparser/arcstandard/weight.cc
    )

target_link_libraries (hc_depparser_hstep_arcstandard _zuo_alphabet _zuo_frontend _zuo_io _zuo_utils ${LIBS})

set_target_properties(hc_depparser_hstep_arcstandard
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/experimental/)

