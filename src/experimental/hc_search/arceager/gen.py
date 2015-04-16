#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import re
import numpy
from optparse import OptionParser

usage = "generate the pairwised training instance for phase two."
parser = OptionParser(usage)
parser.add_option("--language", dest="language", default="en", help="specify the language [default=en]")
parser.add_option("--method", dest="method", default="fine",
        help="specify to generating method [default=fine]")
parser.add_option("--ignore-punctuation", dest="ignore_punctuation", default=False, action="store_true",
        help="specify to include punctuation when calculating loss [default=False].")
opts, args = parser.parse_args()

if opts.method not in ("gold", "coarse", "fine"):
    print >> sys.stderr, "unknown method!"
    sys.exit(1)

if opts.language == "en":
    punc = ("``", ",", ":", ".", "''", "#", "-LRB-", "-RRB-")
elif opts.language == "ch":
    punc = ("PU")
else:
    print >> sys.stderr, "unknown language!"
    sys.exit(1)

def loss(oracle, predict, postags, ignore_punctuation):
    retval = 0
    for i, (o, p) in enumerate(zip(oracle, predict)):
        if ignore_punctuation and postags[i] in punc:
            continue
        if o[0] != p[0] or o[1] != p[1]:
            retval += 1
    return retval

dataset = open(args[0], "r").read().strip().split("\n\n")

for data in dataset:
    lines = data.split("\n")
    header = lines[0]
    mat = [line.split() for line in lines[1:]]
    M, N = len(mat), len(mat[0])
    postags = [mat[r][1] for r in xrange(M)]
    oracle = [mat[r][2].rsplit("/", 1) for r in xrange(M)]
    retval = []

    for i in xrange(3, N):
        predict = [mat[r][i].rsplit("/", 1) for r in xrange(M)]
        retval.append( loss(oracle, predict, postags, opts.ignore_punctuation) )
    ranks = {v: 1 for v in retval}.keys()

    if opts.method == "gold":
        tokens = header.split()
        new_header = tokens[:2] + ["oracle:%s" % tokens[2]]
        mat2 = [line[:3] for line in mat]
        new_header.append("good:%s" % tokens[2])
        for j, row in enumerate(mat2): row.append(mat[j][2])
        for i, token in enumerate(tokens[3:]):
            if retval[i] > 0:
                new_header.append("bad:%s" % token)
                for j, row in enumerate(mat2):
                    row.append(mat[j][i+ 3])
        print " ".join(new_header)
        print "\n".join([" ".join(row) for row in mat2])
        print
    else:
        for l in sorted(ranks)[:-1]:
            tokens = header.split()
            new_header = tokens[:2] + ["oracle:%s" % tokens[2]]
            mat2 = [line[:3] for line in mat]
            for i, token in enumerate(tokens[3:]):
                if retval[i] == l:
                    new_header.append("good:%s" % token)
                    for j, row in enumerate(mat2):
                        row.append(mat[j][i+ 3])
                elif retval[i] > l:
                    new_header.append("bad:%s" % token)
                    for j, row in enumerate(mat2):
                        row.append(mat[j][i+ 3])
            print " ".join(new_header)
            print "\n".join([" ".join(row) for row in mat2])
            print
            if opts.method == "coarse":
                break
