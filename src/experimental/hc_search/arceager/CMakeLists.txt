add_executable (hc_depparser_hstep_arceager
    main.cc
    pipe.h
    pipe.cc
    ${SOURCE_DIR}/experimental/hc_search/hstep_opt_utils.h
    ${SOURCE_DIR}/experimental/hc_search/hstep_opt_utils.cc
    ${SOURCE_DIR}/app/depparser/opt_utils.h
    ${SOURCE_DIR}/app/depparser/opt_utils.cc
    ${SOURCE_DIR}/app/depparser/arceager/decoder.h
    ${SOURCE_DIR}/app/depparser/arceager/decoder.cc
    ${SOURCE_DIR}/app/depparser/arceager/weight.h
    ${SOURCE_DIR}/app/depparser/arceager/weight.cc
    )

target_link_libraries (hc_depparser_hstep_arceager
    _zuo_alphabet _zuo_frontend _zuo_io _zuo_utils _zuo_depparser_arceager ${LIBS})

set_target_properties(hc_depparser_hstep_arceager
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/experimental/)

