add_executable (hc_arceager_depparser
    main.cc
    action.cc
    action_utils.cc
    opt_utils.cc
    state.cc
    score_context.cc
    decoder.cc
    weight.cc
    pipe.cc
    knowledge.cc
    ${SOURCE_DIR}/types/dependency.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/stream.cc
    ${SOURCE_DIR}/utils/io/dataset/dependency.cc
    ${SOURCE_DIR}/utils/io/instance/dependency.cc
    )

target_link_libraries (hc_arceager_depparser
    boost_program_options
    boost_log_setup
    boost_log
    boost_regex
    boost_serialization
    boost_thread
    boost_system
    pthread)

set_target_properties(hc_arceager_depparser
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/experimental/)
