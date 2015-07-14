add_executable (depparser_arceager
    main.cc
    action.cc
    action_utils.cc
    state.cc
    decoder.cc
    weight.cc
    ${SOURCE_DIR}/app/depparser/opt_utils.cc
    ${SOURCE_DIR}/frontend/common_opt_utils.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/stream.cc
    ${SOURCE_DIR}/utils/io/dataset/dependency.cc
    ${SOURCE_DIR}/utils/io/instance/dependency.cc
    ${SOURCE_DIR}/utils/io/instance/csv.cc
    )

target_link_libraries (depparser_arceager
    boost_program_options
    boost_log_setup
    boost_log
    boost_regex
    boost_serialization
    boost_thread
    boost_system
    pthread)

add_executable (depparser_arceager_greedy
    greedy.cc
    greedy_pipe.cc
    action.cc
    action_utils.cc
    state.cc
    decoder.cc
    weight.cc
    ${SOURCE_DIR}/app/depparser/greedy_opt_utils.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/dataset/dependency.cc
    ${SOURCE_DIR}/utils/io/instance/dependency.cc
    ${SOURCE_DIR}/utils/io/instance/csv.cc
    )

target_link_libraries (depparser_arceager_greedy
    boost_program_options
    boost_log_setup
    boost_log
    boost_regex
    boost_serialization
    boost_thread
    boost_system
    pthread)


