#!/bin/bash
#nohup ./run/en-arcstandard-depparser.sh > log/`date +%Y-%m-%d-%H%M%S`.en-arcstandard-depparser.log &
#nohup ./run/en-arceager-depparser.sh > log/`date +%Y-%m-%d-%H%M%S`.en-arceager-depparser.log &
#nohup ./run/wsj-postagger.sh > log/`date +%Y-%m-%d-%H%M%S`.wsj-postagger.log &
#nohup ./run/sequence-labeler-conll2000.sh > log/`date +%Y-%m-%d-%H%M%S`.sequence-labeler-conll2000.log &
#nohup ./run/sequence-labeler-semchunk.sh > log/`date +%Y-%m-%d-%H%M%S`.sequence-labeler-semchunk.log &
#nohup ./run/sequence-labeler-semchunk-minibatch.sh > log/`date +%Y-%m-%d-%H%M%S`.sequence-labeler-semchunk-minibatch.log &
nohup ./run/en-arceager-depparser-minibatch.sh > log/`date +%Y-%m-%d-%H%M%S`.en-arceager-depparser-minibatch.log &
