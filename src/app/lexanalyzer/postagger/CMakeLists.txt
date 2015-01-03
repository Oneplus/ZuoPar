add_executable (postagger
    main.cc
    action.cc
    action_utils.cc
    state.cc
    decoder.cc
    pipe.cc
    weight.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/frontend/common_opt_utils.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/dataset/postag.cc
    ${SOURCE_DIR}/utils/io/instance/postag.cc
    ${SOURCE_DIR}/utils/io/stream.cc
    )

target_link_libraries (postagger
    boost_program_options
    boost_log_setup
    boost_log
    boost_regex
    boost_serialization
    boost_thread
    boost_system
    pthread)
