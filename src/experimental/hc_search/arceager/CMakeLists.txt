add_executable (hc_depparser_hstep_arceager
    main.cc
    pipe.cc
    ${SOURCE_DIR}/experimental/hc_search/hstep_opt_utils.cc
    ${SOURCE_DIR}/frontend/common_opt_utils.cc
    ${SOURCE_DIR}/app/depparser/opt_utils.cc
    ${SOURCE_DIR}/app/depparser/arceager/action.cc
    ${SOURCE_DIR}/app/depparser/arceager/action_utils.cc
    ${SOURCE_DIR}/app/depparser/arceager/state.cc
    ${SOURCE_DIR}/app/depparser/arceager/decoder.cc
    ${SOURCE_DIR}/app/depparser/arceager/weight.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/stream.cc
    ${SOURCE_DIR}/utils/io/dataset/dependency.cc
    ${SOURCE_DIR}/utils/io/instance/csv.cc
    ${SOURCE_DIR}/utils/io/instance/dependency.cc
    )

target_link_libraries (hc_depparser_hstep_arceager ${LIBS})

set_target_properties(hc_depparser_hstep_arceager
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/experimental/)

