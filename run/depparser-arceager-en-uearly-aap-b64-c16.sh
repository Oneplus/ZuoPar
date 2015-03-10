#!/bin/bash
ROOT=`pwd`
TRAIN=${ROOT}/data/dependency/en/train.tab
DEVEL=${ROOT}/data/dependency/en/devi.tab
TEST=${ROOT}/data/dependency/en/testi.tab

DEVEL_ANS=${ROOT}/data/dependency/en/devr.tab
TEST_ANS=${ROOT}/data/dependency/en/testr.tab

SIG=`date '+%Y-%m-%d-%H%M%S'`-depparser-arceager-en-uearly-aap-b64-c16
WORKSPACE=${ROOT}/workspace/${SIG}

MODEL_DIR=${WORKSPACE}/model
OUTPUT_DIR=${WORKSPACE}/output
EXE_DIR=${WORKSPACE}/bin

mkdir -p ${MODEL_DIR}
mkdir -p ${OUTPUT_DIR}
mkdir -p ${EXE_DIR}

MODEL_PREFIX=${MODEL_DIR}/model

EXE=${EXE_DIR}/arceager_depparser.${SIG}
cp ${ROOT}/bin/arceager_depparser ${EXE}

rm ${MODEL_PREFIX}.*

for i in `seq 1 30`; do
    ${EXE} multi-learn \
        -m ${MODEL_PREFIX} \
        -t 12 \
        -d 100 \
        -r ${TRAIN}

    cp ${MODEL_PREFIX} ${MODEL_PREFIX}.${i}
    bzip2 ${MODEL_PREFIX}.${i} &

    ${EXE} test \
        -m ${MODEL_PREFIX} \
        -i ${DEVEL} \
        -o ${OUTPUT_DIR}/devel.${i}

    ${EXE} test \
        -m ${MODEL_PREFIX} \
        -i ${TEST} \
        -o ${OUTPUT_DIR}/test.${i}

    ${ROOT}/scripts/dependency/evaluate.py --language en ${DEVEL_ANS} ${OUTPUT_DIR}/devel.${i}
    ${ROOT}/scripts/dependency/evaluate.py --language en ${TEST_ANS}  ${OUTPUT_DIR}/test.${i}
done
