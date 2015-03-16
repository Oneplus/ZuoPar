add_executable (semchunker_mono_predicate
    main.cc
    information.cc
    opt_utils.cc
    argument_relation_utils.cc
    pipe.cc
    action.cc
    action_utils.cc
    state.cc
    decoder.cc
    weight.cc
    ${SOURCE_DIR}/frontend/common_opt_utils.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/dataset/semchunks.cc
    ${SOURCE_DIR}/utils/io/instance/csv.cc
    ${SOURCE_DIR}/utils/io/instance/semchunks.cc
    ${SOURCE_DIR}/utils/io/stream.cc
    )

target_link_libraries (semchunker_mono_predicate
    boost_program_options
    boost_log_setup
    boost_log
    boost_regex
    boost_serialization
    boost_thread
    boost_system
    pthread)
