#!/bin/bash
ROOT=`pwd`
TRAIN=${ROOT}/data/dependency/en/train.txt
DEVEL=${ROOT}/data/dependency/en/devr.txt
TEST=${ROOT}/data/dependency/en/testr.txt

DEVEL_ANS=${ROOT}/data/dependency/en/devr.conll
TEST_ANS=${ROOT}/data/dependency/en/testr.conll

SIG=`date '+%Y-%m-%d-%H%M%S'`-en-arcstandard-depparser
WORKSPACE=${ROOT}/workspace/${SIG}

MODEL_DIR=${WORKSPACE}/model
OUTPUT_DIR=${WORKSPACE}/output
EXE_DIR=${WORKSPACE}/bin

mkdir -p ${MODEL_DIR}
mkdir -p ${OUTPUT_DIR}
mkdir -p ${EXE_DIR}

MODEL_PREFIX=${MODEL_DIR}/model

EXE=${EXE_DIR}/arcstandard_depparser.${SIG}
cp ${ROOT}/bin/arcstandard_depparser ${EXE}

rm ${MODEL_PREFIX}.*

for i in `seq 1 20`; do
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

    ${ROOT}/scripts/dependency/simplified2conll.py \
        ${OUTPUT_DIR}/devel.${i} > ${OUTPUT_DIR}/devel.conll.${i}
    ${ROOT}/scripts/dependency/simplified2conll.py \
        ${OUTPUT_DIR}/test.${i} > ${OUTPUT_DIR}/test.conll.${i}

    ${ROOT}/scripts/dependency/eval.pl -q -g ${DEVEL_ANS} -s ${OUTPUT_DIR}/devel.conll.${i}
    ${ROOT}/scripts/dependency/eval.pl -q -g ${TEST_ANS} -s ${OUTPUT_DIR}/test.conll.${i}
done
