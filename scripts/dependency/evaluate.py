#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import re
from optparse import OptionParser

usage = "Script for parsing accuracy evaluation on tab file."
parser = OptionParser(usage)
parser.add_option("--punctuation", dest="punctuation", default=False, action="store_true", help="specify to include punctuation in evaluation.")
parser.add_option("--language", dest="language", help="specify language")
opts, args = parser.parse_args()

if opts.language == "en":
    # English punctuation list is obtained from http://en.wikipedia.org/wiki/Punctuation_of_English
    engine = lambda x: x in ("'", "''", # apostrophe
            "(", ")", "[", "]", "{", "}", "-LRB-", "-RRB-", "-LSB-", "-RSB-", "-LCB-", "-RCB-", # brackets
            ":", # colon
            ",", # comma
            "-", "--", # dash
            "...", # ellipsis
            "!", # exclamation mark
            ".", # full stop
            "\"", "``", "`", # quotation marks
            ";", # semicolon
            "?" # question mark
            )
elif opts.language == "ch":
    engine = lambda x: x in (
            "（", "）",
            "、", "。", "．",
            "！",
            "？",
            "…", "，", "；", "／", "：",
            "“", "”", "‘", "’", 
            "「", "」", "『", "』", "《", "》", "〈", "〉",
            "一一", "――", "―", 
            )
else:
    print >> sys.stderr, "Unknown language"
    sys.exit(1)

reference_dataset = open(args[0], "r").read().strip().split("\n\n")
answer_dataset = open(args[1], "r").read().strip().split("\n\n")

assert len(reference_dataset) == len(answer_dataset), "Number of instance unequal."

details = {}

for reference_data, answer_data in zip(reference_dataset, answer_dataset):
    reference_lines = reference_data.split("\n")
    answer_lines = answer_data.split("\n")

    assert len(reference_lines) == len(answer_lines), "Number of words unequal."
    for reference_line, answer_line in zip(reference_lines, answer_lines):
        reference_word, reference_postag, reference_head, reference_deprel = reference_line.strip().split()
        answer_word, answer_postag, answer_head, answer_deprel = answer_line.strip().split()
        assert reference_word == answer_word, "Word not equal."
        if not opts.punctuation and engine(reference_word):
            continue
        if reference_postag not in details:
            details[reference_postag] = [0, 0, 0]
        details[reference_postag][2] += 1
        if reference_head == answer_head:
            details[reference_postag][0] += 1
            if reference_deprel == answer_deprel:
                details[reference_postag][1] += 1

print "%10s%10s%10s%10s" % ("postag", "corr.hed", "corr.rel", "tot.rel")
print "".join('-' for i in xrange(60))
for postag, detail in sorted(details.iteritems()):
    print "%10s%10d%10d%10d %f %f" % (postag, detail[0], detail[1], detail[2], float(detail[0]) /detail[2], float(detail[1])/detail[2])

corr_head = sum([int(val[0]) for key, val in details.iteritems()])
corr_rel = sum([int(val[1]) for key, val in details.iteritems()])
tot_rel = sum([int(val[2]) for key, val in details.iteritems()])

print "".join('-' for i in xrange(60))
print "%10s%10d%10d%10d %f %f" % ("Overall", corr_head, corr_rel, tot_rel, float(corr_head) /tot_rel, float(corr_rel)/tot_rel)
