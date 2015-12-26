add_executable (depparser_arcstandard
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

target_link_libraries (depparser_arcstandard ${LIBS})

add_executable (depparser_arcstandard_greedy
    greedy.cc
    action.cc
    action_utils.cc
    state.cc
    decoder.cc
    weight.cc
    ${SOURCE_DIR}/app/depparser/greedy_opt_utils.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/stream.cc
    ${SOURCE_DIR}/utils/io/dataset/dependency.cc
    ${SOURCE_DIR}/utils/io/instance/dependency.cc
    ${SOURCE_DIR}/utils/io/instance/csv.cc
    )

target_link_libraries (depparser_arcstandard_greedy ${LIBS})

