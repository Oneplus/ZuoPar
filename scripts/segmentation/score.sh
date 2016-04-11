#!/bin/bash
root=`dirname $0`
$root/score /dev/null $1 $2 | egrep '=== F' | awk '{print $4}'
