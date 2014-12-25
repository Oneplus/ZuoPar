#!/usr/bin/env python
import sys
from optparse import OptionParser

usage = "%s -g <answer> -s <system>" % (sys.argv[0])
parser = OptionParser(usage)

parser.add_option("-g", "--gold", dest="gold", help="the gold result")
parser.add_option("-s", "--system", dest="system", help="the system output")
opts, args = parser.parse_args()

def read(filename):
    return [[token.rsplit("/", 1) for token in inst.split()]
            for inst in open(filename, "r").read().split("\n")]

answer = read(opts.gold)
system = read(opts.system)

assert (len(answer) == len(system))

nr_recalled, nr_total = 0, 0
for answ, syst in zip(answer, system):
    assert (len(answ) == len(syst))
    for (F0, P0), (F1, P1) in zip(answ, syst):
        assert (F0 == F1)
        if P0 == P1:
            nr_recalled += 1
        nr_total += 1

print "Precision = %f ( %d / %d )" % ((float(nr_recalled)/ nr_total),
        nr_recalled, nr_total)
