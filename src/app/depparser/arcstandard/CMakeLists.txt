add_library (_zuo_depparser_arcstandard
    action.h
    action.cc
    action_utils.h
    action_utils.cc
    state.h
    state.cc
    )

add_executable (depparser_arcstandard
    main.cc
    decoder.h
    decoder.cc
    weight.h
    weight.cc
    ${SOURCE_DIR}/app/depparser/pipe.h
    ${SOURCE_DIR}/app/depparser/multi_pipe.h
    ${SOURCE_DIR}/app/depparser/opt_utils.h
    ${SOURCE_DIR}/app/depparser/opt_utils.cc
    )

target_link_libraries (depparser_arcstandard
    _zuo_alphabet
    _zuo_frontend
    _zuo_io
    _zuo_utils
    _zuo_depparser_arcstandard
    ${LIBS}
    )

add_executable (depparser_arcstandard_greedy
    greedy.cc
    decoder.h
    decoder.cc
    weight.h
    weight.cc
    ${SOURCE_DIR}/app/depparser/pipe.h
    ${SOURCE_DIR}/app/depparser/greedy_opt_utils.h
    ${SOURCE_DIR}/app/depparser/greedy_opt_utils.cc
    )

target_link_libraries (depparser_arcstandard_greedy
    _zuo_alphabet
    _zuo_frontend
    _zuo_io
    _zuo_utils
    _zuo_depparser_arcstandard
    ${LIBS}
    )

