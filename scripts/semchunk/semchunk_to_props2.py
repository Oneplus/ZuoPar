#!/usr/bin/env python
import sys
import re
from optparse import OptionParser

try:
    fp=open(sys.argv[1], "r")
except:
    fp=sys.stdin
for data in fp.read().strip().split("\n\n"):
    tags = [line.strip().split()[0] for line in data.split("\n")]
    output = ["*" for _ in xrange(len(tags))]
    for i, tag in enumerate(tags):
        if tag.startswith("B-") or tag == "O":
            if i - 1 >= 0 and tags[i- 1][:2] in ("B-", "I-"):
                output[i- 1] = output[i- 1] + ")"
        if tag.startswith("B-") or (tag.startswith("I-") and (i < 1 or tags[i-1] == "O")):
            arglabel = tag.split("-", 1)[1]
            if arglabel == "rel":
                arglabel = "V"
            output[i] = ("(%s" % arglabel) + output[i]
    if tags[i][:2] in ("B-", "I-"):
        output[i] = output[i] + ")"
    print "\n".join(output)
    print
