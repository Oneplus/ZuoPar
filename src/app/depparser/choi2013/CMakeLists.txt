add_executable (depparser_choi2013
    main.cc
    action.h
    action.cc
    action_utils.h
    action_utils.cc
    state.h
    state.cc
    decoder.h
    decoder.cc
    weight.h
    weight.cc
    score_context.h
    score_context.cc
    ${SOURCE_DIR}/app/depparser/pipe.h
    ${SOURCE_DIR}/app/depparser/multi_pipe.h
    ${SOURCE_DIR}/app/depparser/opt_utils.h
    ${SOURCE_DIR}/app/depparser/opt_utils.cc
    )

target_link_libraries (depparser_choi2013 _zuo_alphabet _zuo_frontend _zuo_io _zuo_utils ${LIBS})

add_executable (depparser_choi2013_greedy
    greedy.cc
    action.h
    action.cc
    action_utils.h
    action_utils.cc
    state.h
    state.cc
    decoder.h
    decoder.cc
    weight.h
    weight.cc
    score_context.h
    score_context.cc
    ${SOURCE_DIR}/app/depparser/pipe.h
    ${SOURCE_DIR}/app/depparser/greedy_opt_utils.h
    ${SOURCE_DIR}/app/depparser/greedy_opt_utils.cc
    )

target_link_libraries(depparser_choi2013_greedy _zuo_alphabet _zuo_frontend _zuo_io _zuo_utils ${LIBS})

