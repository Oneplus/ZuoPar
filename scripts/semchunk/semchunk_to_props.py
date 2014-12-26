#!/usr/bin/env python
import sys
import re
from optparse import OptionParser

usage = '''Use to convert the data in semchunk format to props format used
CoNLL2004 and CoNLL2005 benchmark.'''
parser = OptionParser(usage)
parser.add_option("--text", dest="text", help="the text file.")
parser.add_option("--tag", dest="tag", help="the tag file (optional, if not provided use 5nd column as tag).")
opts, args = parser.parse_args()

references = [data.split("\n") for data in open(opts.text, "r").read().strip().split("\n\n")]
if opts.tag is not None:
    answers = [tag.split("\n") for tag in open(opts.tag, "r").read().strip().split("\n\n")]
else:
    answers = [[_.split()[4] for _ in reference[1:]] for reference in references]

assert len(references) == len(answers), "number of ref and answer not equal."

dataset = {}

for reference, answer in zip(references, answers):
    info = reference[0]
    assert len(reference[1:]) == len(answer), "number of tokens not equal."
    section, sid = info.lstrip("#").strip().split()
    section_id = int(re.search("chtb_(\d+)", section).group(1))
    sid = int(sid)
    if (section_id, sid) not in dataset:
        dataset[section_id, sid] = []
    dataset[section_id, sid].append( [reference[1:], answer] )

for (section_id, sid), payloads in sorted(dataset.iteritems(), key=lambda x: x[0]):
    M = len(payloads[0][0])
    for payload in payloads:
        assert len(payload[0]) == M, "number of tokens in reference not equal."
        assert len(payload[1]) == M, "number of tokens in answer not equal."
    for payload in payloads:
        payload[0] = [_.split() for _ in payload[0]]

    apreds = sorted(payloads, key=lambda x: [0 if _[3] == "_" else 1 for _ in x[0]].index(1))
    N = len(apreds) + 1
    output = [["-" if __ == 0 else "*" for _ in xrange(M)] for __ in xrange(N)]
    for i, (ref, ans) in enumerate(apreds):
        predidx = [0 if _[3] == "_" else 1 for _ in ref].index(1)
        output[0][predidx] = ref[predidx][3]
        for j, a in enumerate(ans):
            if a.startswith("B-") or a == "O":
                if j - 1 >= 0 and ans[j- 1][:2] in ("B-", "I-"):
                    output[i+ 1][j- 1] = output[i+ 1][j- 1] + ")"
            if a.startswith("B-") or (a.startswith("I-") and (j < 1 or ans[j-1] == "O")):
                arglabel = a.split("-", 1)[1]
                if arglabel == "rel":
                    arglabel = "V"
                output[i+ 1][j] = ("(%s" % arglabel) + output[i+ 1][j]
        if ans[j][:2] in ("B-", "I-"):
            output[i+ 1][j] = output[i+ 1][j] + ")"
    #print output
    print "\n".join(["\t".join(_) for _ in zip(*output)])
    print
