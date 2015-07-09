add_executable (hc_depparser_hstep_arcstandard
    main.cc
    pipe.cc
    ${SOURCE_DIR}/experimental/hc_search/hstep_opt_utils.cc
    ${SOURCE_DIR}/frontend/common_opt_utils.cc
    ${SOURCE_DIR}/app/depparser/opt_utils.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/action.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/action_utils.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/state.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/decoder.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/weight.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/stream.cc
    ${SOURCE_DIR}/utils/io/dataset/dependency.cc
    ${SOURCE_DIR}/utils/io/instance/csv.cc
    ${SOURCE_DIR}/utils/io/instance/dependency.cc
    )

target_link_libraries (hc_depparser_hstep_arcstandard
    boost_program_options
    boost_log_setup
    boost_log
    boost_regex
    boost_serialization
    boost_thread
    boost_system
    pthread)

set_target_properties(hc_depparser_hstep_arcstandard
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/experimental/)

