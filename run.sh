#!/bin/bash
#nohup ./run/en-arcstandard-depparser.sh           > log/`date +%Y-%m-%d-%H%M%S`.en-arcstandard-depparser.log &
#nohup ./run/en-arcstandard-depparser-minibatch.sh > log/`date +%Y-%m-%d-%H%M%S`.en-arcstandard-depparser-minibatch.log &
#nohup ./run/en-arceager-depparser.sh              > log/`date +%Y-%m-%d-%H%M%S`.en-arceager-depparser.log &
#nohup ./run/en-arceager-depparser-minibatch.sh    > log/`date +%Y-%m-%d-%H%M%S`.en-arceager-depparser-minibatch.log &
#nohup ./run/wsj-postagger.sh                      > log/`date +%Y-%m-%d-%H%M%S`.wsj-postagger.log &
#nohup ./run/seqlabeler-conll2000.sh               > log/`date +%Y-%m-%d-%H%M%S`.seqlabeler-conll2000.log &
nohup ./run/seqlabeler-semchunk-ch.sh             > log/`date +%Y-%m-%d-%H%M%S`.seqlabeler-semchunk-ch.log &
nohup ./run/seqlabeler-semchunk-ch-minibatch.sh   > log/`date +%Y-%m-%d-%H%M%S`.seqlabeler-semchunk-ch-minibatch.log &
#nohup ./run/seqlabeler-semchunk-en.sh             > log/`date +%Y-%m-%d-%H%M%S`.seqlabeler-semchunk-en.log &
#nohup ./run/seqlabeler-semchunk-en-minibatch.sh   > log/`date +%Y-%m-%d-%H%M%S`.seqlabeler-semchunk-en-minibatch.log &
#nohup ./run/mono_srl.sh                           > log/`date +%Y-%m-%d-%H%M%S`.mono_srl.log &

