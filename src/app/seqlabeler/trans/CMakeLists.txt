add_executable (sequence_labeler
    main.cc
    opt_utils.h
    opt_utils.cc
    action.h
    action.cc
    action_utils.h
    action_utils.cc
    state.h
    state.cc
    decoder.h
    decoder.cc
    pipe.h
    pipe.cc
    multi_pipe.h
    multi_pipe.cc
    weight.h
    weight.cc
    )

target_link_libraries (sequence_labeler _zuo_alphabet _zuo_frontend _zuo_io _zuo_utils ${LIBS})
