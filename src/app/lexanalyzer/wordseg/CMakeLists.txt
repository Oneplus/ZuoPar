add_executable (wordseg
    main.cc
    action.h
    action.cc
    action_utils.h
    action_utils.cc
    state.h
    state.cc
    score_context.h
    score_context.cc
    decoder.h
    decoder.cc
    pipe.h
    pipe.cc
    weight.h
    weight.cc
    )

target_link_libraries (wordseg _zuo_alphabet _zuo_frontend _zuo_io _zuo_utils ${LIBS})
