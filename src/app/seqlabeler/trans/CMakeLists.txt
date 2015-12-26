add_executable (sequence_labeler
    main.cc
    opt_utils.cc
    action.cc
    action_utils.cc
    state.cc
    decoder.cc
    pipe.cc
    multi_pipe.cc
    weight.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/frontend/common_opt_utils.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/dataset/sequence_instance.cc
    ${SOURCE_DIR}/utils/io/instance/sequence_instance.cc
    ${SOURCE_DIR}/utils/io/instance/csv.cc
    ${SOURCE_DIR}/utils/io/stream.cc
    )

target_link_libraries (sequence_labeler ${LIBS})
