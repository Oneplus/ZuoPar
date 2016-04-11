#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import re
import unicodedata
from optparse import OptionParser

UNICODEPUNC = dict.fromkeys(i for i in xrange(sys.maxunicode)
        if unicodedata.category(unichr(i)).startswith('P'))
usage = "Script for parsing accuracy evaluation on tab file.\n ./%s [options] reference answer" % sys.argv[0]
parser = OptionParser(usage)
parser.add_option("--conllx", dest="conllx", default=False, action="store_true", help="specify the conllx data format.")
parser.add_option("--punctuation", dest="punctuation", default=False, action="store_true", help="specify to include punctuation in evaluation.")
parser.add_option("--ignore", dest="ignore", default=None, help="ignore form")
parser.add_option("--language", dest="language", help="specify language [en,ch,universal,chen2014en,chen2014ch]")
parser.add_option("--quite", dest="quite", default=False, action="store_true", help="specify not out details.")
opts, args = parser.parse_args()

if opts.language == "en":
    # English punctuation list is obtained from http://en.wikipedia.org/wiki/Punctuation_of_English
    engine = lambda x, y: x in ("'", "''", # apostrophe
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
            ) or x == opts.ignore
elif opts.language == "ch":
    engine = lambda x, y: x in (
            "（", "）",
            "、", "。", "．",
            "！",
            "？",
            "…", "，", "；", "／", "：",
            "“", "”", "‘", "’", 
            "「", "」", "『", "』", "《", "》", "〈", "〉",
            "一一", "――", "―", 
            ) or x == opts.ignore
elif opts.language == "universal":
    engine = lambda x, y: len(x.decode("utf-8").translate(UNICODEPUNC)) == 0 or x == opts.ignore
elif opts.language == "chen2014en":
    engine = lambda x, y: y in set(["''", "``", ",", ".", ":"])
elif opts.language == "chen2014ch":
    engine = lambda x, y: y in set(['PU'])
else:
    print >> sys.stderr, "Unknown language"
    sys.exit(1)

reference_dataset = open(args[0], "r").read().strip().split("\n\n")
if args[1] != '-':
    answer_dataset = open(args[1], "r").read().strip().split("\n\n")
else:
    answer_dataset = sys.stdin.read().strip().split('\n\n')

assert len(reference_dataset) == len(answer_dataset), "Number of instance unequal."

details = {}

for reference_data, answer_data in zip(reference_dataset, answer_dataset):
    reference_lines = reference_data.split("\n")
    answer_lines = answer_data.split("\n")

    assert len(reference_lines) == len(answer_lines), "Number of words unequal."
    for reference_line, answer_line in zip(reference_lines, answer_lines):
        if opts.conllx:
            payload = reference_line.strip().split()
            reference_word, reference_postag, reference_head, reference_deprel = payload[1], payload[3], payload[6], payload[7]
            payload = answer_line.strip().split()
            answer_word, answer_postag, answer_head, answer_deprel = payload[1], payload[3], payload[6], payload[7]
        else:
            reference_word, reference_postag, reference_head, reference_deprel = reference_line.strip().split()
            answer_word, answer_postag, answer_head, answer_deprel = answer_line.strip().split()
        assert reference_word == answer_word, "Word not equal."
        if not opts.punctuation and engine(reference_word, reference_postag):
            continue
        if reference_postag not in details:
            details[reference_postag] = [0, 0, 0]
        details[reference_postag][2] += 1
        if reference_head == answer_head:
            details[reference_postag][0] += 1
            if reference_deprel == answer_deprel:
                details[reference_postag][1] += 1

print "%10s%10s%10s%10s" % ("postag", "corr.hed", "corr.rel", "tot.rel")
if not opts.quite:
    print "".join('-' for i in xrange(60))
    for postag, detail in sorted(details.iteritems()):
        print "%10s%10d%10d%10d %f %f" % (postag, detail[0], detail[1], detail[2], float(detail[0]) /detail[2], float(detail[1])/detail[2])

corr_head = sum([int(val[0]) for key, val in details.iteritems()])
corr_rel = sum([int(val[1]) for key, val in details.iteritems()])
tot_rel = sum([int(val[2]) for key, val in details.iteritems()])

print "".join('-' for i in xrange(60))
print "%10s%10d%10d%10d %f %f" % ("Overall", corr_head, corr_rel, tot_rel, float(corr_head) /tot_rel, float(corr_rel)/tot_rel)
