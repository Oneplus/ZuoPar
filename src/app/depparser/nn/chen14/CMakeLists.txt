include_directories (
    ${THIRDPARTY_DIR}/armadillo-5.100.1/include/)

add_executable (nndepparser_chen14
    main.cc
    pipe.h
    pipe.cc
    opt_utils.h
    opt_utils.cc
    classifier.h
    classifier.cc
    ${SOURCE_DIR}/app/depparser/nn/dataset.cc
    ${SOURCE_DIR}/app/depparser/nn/decoder.cc
    )

target_link_libraries (nndepparser_chen14
    _zuo_alphabet
    _zuo_frontend
    _zuo_io
    _zuo_utils
    _zuo_depparser_arcstandard
    openblas
    lapack
    icuuc
    pthread
    ${LIBS}
    )
