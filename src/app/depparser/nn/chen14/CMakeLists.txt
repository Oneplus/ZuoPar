include_directories (
    ${THIRDPARTY_DIR}/armadillo-5.100.1/include/)

add_executable (nndepparser_chen14
    main.cc
    pipe.cc
    opt_utils.cc
    classifier.cc
    ${SOURCE_DIR}/app/depparser/nn/dataset.cc
    ${SOURCE_DIR}/app/depparser/nn/decoder.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/action.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/action_utils.cc
    ${SOURCE_DIR}/app/depparser/arcstandard/state.cc
    ${SOURCE_DIR}/frontend/common_opt_utils.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/stream.cc
    ${SOURCE_DIR}/utils/io/dataset/dependency.cc
    ${SOURCE_DIR}/utils/io/instance/dependency.cc
    ${SOURCE_DIR}/utils/io/instance/csv.cc
    )

target_link_libraries (nndepparser_chen14
    boost_program_options
    boost_log_setup
    boost_log
    boost_regex
    boost_serialization
    boost_thread
    boost_timer
    boost_system
    openblas
    lapack
    icuuc
    pthread)
