include_directories (
    ${THIRDPARTY_DIR}/armadillo-5.100.1/include/)

add_executable (nndepparser_weiss15
    main.cc
    pipe.h
    pipe.cc
    opt_utils.h
    opt_utils.cc
    classifier.h
    classifier.cc
    model.h
    model.cc
    search.h
    search.cc
    ${SOURCE_DIR}/app/depparser/nn/extractor.cc
    ${SOURCE_DIR}/app/depparser/nn/decoder.cc
    ${SOURCE_DIR}/app/depparser/nn/dataset.cc
    )

target_link_libraries (nndepparser_weiss15
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
