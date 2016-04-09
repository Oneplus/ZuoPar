add_executable (depparser_swap
    main.cc
    action.h
    action.cc
    action_utils.h
    action_utils.cc
    score_context.h
    score_context.cc
    state.h
    state.cc
    decoder.h
    decoder.cc
    weight.h
    weight.cc
    ${SOURCE_DIR}/app/depparser/opt_utils.h
    ${SOURCE_DIR}/app/depparser/opt_utils.cc
    )

target_link_libraries (depparser_swap _zuo_alphabet _zuo_frontend _zuo_io _zuo_utils ${LIBS})

add_executable (depparser_swap_greedy
    greedy.cc
    action.h
    action.cc
    action_utils.h
    action_utils.cc
    score_context.h
    score_context.cc
    state.h
    state.cc
    decoder.h
    decoder.cc
    weight.h
    weight.cc
    ${SOURCE_DIR}/app/depparser/greedy_opt_utils.h
    ${SOURCE_DIR}/app/depparser/greedy_opt_utils.cc
    )

target_link_libraries(depparser_swap_greedy _zuo_alphabet _zuo_frontend _zuo_io _zuo_utils ${LIBS})
