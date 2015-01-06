#!/bin/bash
ROOT=`pwd`
TRAIN=${ROOT}/data/semchunk/en/train.semchunk.crfsuite.txt
DEVEL=${ROOT}/data/semchunk/en/devel.semchunk.crfsuite.txt
TEST=${ROOT}/data/semchunk/en/test.semchunk.crfsuite.txt
CONSTRAIN=${ROOT}/data/semchunk/en/trans

DEVEL_PROPS=${ROOT}/data/semchunk/en/devel.answer.props
TEST_PROPS=${ROOT}/data/semchunk/en/test.answer.props

SIG=`date '+%Y-%m-%d-%H%M%S'`-seqlabeler-semchunk-en-uearly-aap-b64-c01-n0
WORKSPACE=${ROOT}/workspace/${SIG}

MODEL_DIR=${WORKSPACE}/model
OUTPUT_DIR=${WORKSPACE}/output
EXE_DIR=${WORKSPACE}/bin

mkdir -p ${MODEL_DIR}
mkdir -p ${OUTPUT_DIR}
mkdir -p ${EXE_DIR}

MODEL_PREFIX=${MODEL_DIR}/model

EXE=${EXE_DIR}/sequence_labeler.${SIG}
cp ${ROOT}/bin/sequence_labeler ${EXE}

rm ${MODEL_PREFIX}.*

for i in `seq 1 60`; do
    ${EXE} learn \
        -m ${MODEL_PREFIX} \
        -r ${TRAIN}

    cp ${MODEL_PREFIX} ${MODEL_PREFIX}.${i}
    bzip2 ${MODEL_PREFIX}.${i}

    ${EXE} test \
        -m ${MODEL_PREFIX} \
        -i ${DEVEL} \
        -o ${OUTPUT_DIR}/devel.${i}

    ${EXE} test \
        -m ${MODEL_PREFIX} \
        -i ${TEST} \
        -o ${OUTPUT_DIR}/test.${i}

    TMP=/tmp/`date +%M%H%S | base64`
    ${ROOT}/scripts/semchunk/semchunk_to_props2.py ${OUTPUT_DIR}/devel.${i} > ${TMP}
    awk '{print $1}' ${DEVEL_PROPS} | paste -d "\t" - ${TMP} | sed 's/^\t$//g' > ${OUTPUT_DIR}/devel.${i}.props


    ${ROOT}/scripts/semchunk/semchunk_to_props2.py ${OUTPUT_DIR}/test.${i} > ${TMP}
    awk '{print $1}' ${TEST_PROPS} | paste -d "\t" - ${TMP} | sed 's/^\t$//g' > ${OUTPUT_DIR}/test.${i}.props
    rm ${TMP}

    ${ROOT}/scripts/semchunk/srl-eval.pl ${DEVEL_PROPS} ${OUTPUT_DIR}/devel.${i}.props
    ${ROOT}/scripts/semchunk/srl-eval.pl ${TEST_PROPS}  ${OUTPUT_DIR}/test.${i}.props
done
