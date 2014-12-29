add_executable (mono_srl
    main.cc
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
    ${SOURCE_DIR}/utils/io/instance/semchunks.cc
    ${SOURCE_DIR}/utils/io/stream.cc
    )

target_link_libraries (mono_srl
    boost_program_options
    boost_log_setup
    boost_log
    boost_regex
    boost_serialization
    boost_thread
    boost_system
    pthread)

set_target_properties(mono_srl PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/experimental/)
