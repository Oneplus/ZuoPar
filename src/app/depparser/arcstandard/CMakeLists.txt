add_executable (arcstandard_depparser
    main.cc
    learn.cc
    test.cc
    opt_utils.cc
    action.cc
    action_utils.cc
    state.cc
    decoder.cc
    pipe.cc
    weight.cc
    ${SOURCE_DIR}/types/dependency.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/dataset.cc
    ${SOURCE_DIR}/utils/io/instance.cc
    )

target_link_libraries (arcstandard_depparser
    boost_program_options
    boost_log_setup
    boost_log
    boost_regex
    boost_serialization
    boost_thread
    boost_system
    pthread)
