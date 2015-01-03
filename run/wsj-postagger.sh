#!/bin/bash
ROOT=`pwd`
TRAIN=${ROOT}/data/postag/wsj/train.txt
DEVEL=${ROOT}/data/postag/wsj/devel.txt
TEST=${ROOT}/data/postag/wsj/test.txt

DEVEL_ANS=${ROOT}/data/postag/wsj/devel.txt
TEST_ANS=${ROOT}/data/postag/wsj/test.txt

SIG=`date '+%Y-%m-%d-%H%M%S'`-wsj-postagger
WORKSPACE=${ROOT}/workspace/${SIG}

MODEL_DIR=${WORKSPACE}/model
OUTPUT_DIR=${WORKSPACE}/output
EXE_DIR=${WORKSPACE}/bin

mkdir -p ${MODEL_DIR}
mkdir -p ${OUTPUT_DIR}
mkdir -p ${EXE_DIR}

MODEL_PREFIX=${MODEL_DIR}/model

EXE=${EXE_DIR}/postagger.${SIG}
cp ${ROOT}/bin/postagger ${EXE}

rm ${MODEL_PREFIX}.*

for i in `seq 1 10`; do
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

    ${ROOT}/scripts/postag/eval.py -g ${DEVEL_ANS} -s ${OUTPUT_DIR}/devel.${i}
    ${ROOT}/scripts/postag/eval.py -g ${TEST_ANS} -s ${OUTPUT_DIR}/test.${i}
done
