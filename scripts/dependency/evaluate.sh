#!/bin/bash
root=`dirname $0`;
$root/evaluate.py --conllx --language=$1 $2 $3 --quite | egrep 'Overall' | awk '{print $5}'
