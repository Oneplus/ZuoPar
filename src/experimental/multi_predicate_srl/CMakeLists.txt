add_executable (multi_predicate_srl
    main.cc
    opt_utils.cc
    pipe.cc
    argument_relation_utils.cc
    action.cc
    action_utils.cc
    state.cc
    score_context.cc
    information.cc
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

target_link_libraries (multi_predicate_srl ${LIBS})

set_target_properties (multi_predicate_srl
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/experimental/)
