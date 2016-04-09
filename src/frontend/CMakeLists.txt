set (frontend_SRC common_opt_utils.h
    common_opt_utils.cc
    common_pipe_utils.h
    common_pipe_utils.cc)
    
add_library (_zuo_frontend ${frontend_SRC})