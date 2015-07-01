add_executable (conparser main.cc
    pipe.cc
    opt_utils.cc
    ${SOURCE_DIR}/frontend/common_opt_utils.cc
    ${SOURCE_DIR}/engine/token_alphabet.cc
    ${SOURCE_DIR}/utils/logging.cc
    ${SOURCE_DIR}/utils/io/dataset/constituent.cc
    ${SOURCE_DIR}/utils/io/instance/constituent.cc
    ${SOURCE_DIR}/utils/io/instance/lisp.cc
    )

target_link_libraries (conparser
    boost_program_options
    boost_log_setup
    boost_log
    boost_regex
    boost_serialization
    boost_thread
    boost_system
    pthread)
