#!/bin/bash
ROOT=`pwd`
TRAIN=${ROOT}/data/chunking/train.crfsuite.txt
TEST=${ROOT}/data/chunking/test.crfsuite.txt

TEST_ANS=${ROOT}/data/chunking/test.txt

SIG=`date '+%Y-%m-%d-%H%M%S'`-seqlabeler-conll00
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

for i in `seq 1 10`; do
    ${EXE} learn \
        -m ${MODEL_PREFIX} \
        -r ${TRAIN}

    tar zcvf ${MODEL_PREFIX}.${i}.tgz ${MODEL_PREFIX}

    ${EXE} test \
        -m ${MODEL_PREFIX} \
        -i ${TEST} \
        -o ${OUTPUT_DIR}/test.${i}

    paste -d ' ' ${TEST_ANS} ${OUTPUT_DIR}/test.${i} | sed 's/^ $//g' | ${ROOT}/scripts/chunking/conlleval.pl
done
