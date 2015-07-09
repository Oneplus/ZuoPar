add_subdirectory (arceager)
add_subdirectory (arcstandard)

add_executable (hc_depparser_cstep
    main.cc
    pipe.cc
    weight.cc
    #knowledge.cc
    score_context.cc
    cstep_opt_utils.cc
    ${SOURCE_DIR}/frontend/common_opt_utils.cc
    ${SOURCE_DIR}/app/depparser/opt_utils.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/instance/csv.cc
    )

target_link_libraries (hc_depparser_cstep
    boost_program_options
    boost_log_setup
    boost_log
    boost_regex
    boost_serialization
    boost_thread
    boost_system
    icuuc
    pthread)

set_target_properties(hc_depparser_cstep
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/experimental/)

