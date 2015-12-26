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

target_link_libraries (postagger ${LIBS})
