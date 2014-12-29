#!/bin/bash
ROOT=`pwd`
TRAIN=${ROOT}/data/semchunk/train.semchunk.crfsuite.txt
DEVEL=${ROOT}/data/semchunk/devel.semchunk.crfsuite.txt
TEST=${ROOT}/data/semchunk/test.semchunk.crfsuite.txt

DEVEL_RAW=${ROOT}/data/semchunk/devel.semchunk.txt
TEST_RAW=${ROOT}/data/semchunk/test.semchunk.txt

DEVEL_PROPS=${ROOT}/data/semchunk/devel.semchunk.props
TEST_PROPS=${ROOT}/data/semchunk/test.semchunk.props

SIG=`date '+%Y-%m-%d-%H%M%S'`-seqlabeler-semchunk
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

for i in `seq 1 50`; do
    ${EXE} learn \
        -m ${MODEL_PREFIX} \
        -r ${TRAIN}

    tar zcvf ${MODEL_PREFIX}.${i}.tgz ${MODEL_PREFIX}

    ${EXE} test \
        -m ${MODEL_PREFIX} \
        -i ${DEVEL} \
        -o ${OUTPUT_DIR}/devel.${i}

    ${EXE} test \
        -m ${MODEL_PREFIX} \
        -i ${TEST} \
        -o ${OUTPUT_DIR}/test.${i}

    ${ROOT}/scripts/semchunk/semchunk_to_props.py --text ${DEVEL_RAW} \
        --tag ${OUTPUT_DIR}/devel.${i} > ${OUTPUT_DIR}/devel.${i}.props
    ${ROOT}/scripts/semchunk/semchunk_to_props.py --text ${TEST_RAW} \
        --tag ${OUTPUT_DIR}/test.${i} > ${OUTPUT_DIR}/test.${i}.props

    ${ROOT}/scripts/semchunk/srl-eval.pl ${DEVEL_PROPS} ${OUTPUT_DIR}/devel.${i}.props
    ${ROOT}/scripts/semchunk/srl-eval.pl ${TEST_PROPS}  ${OUTPUT_DIR}/test.${i}.props
done
