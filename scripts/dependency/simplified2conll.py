#!/usr/bin/env python
import sys
if __name__=="__main__":
    for nr, instance in enumerate(open(sys.argv[1], "r").read().strip().split("\n\n")):
        if nr != 0:
            print
        for idx, line in enumerate(instance.split("\n")):
            form, postag, head, deprel = line.strip().split()
            output = [
                    str(idx + 1),       # id
                    form,               # form
                    "_",                # lemma
                    postag,             # postag
                    "_",                # cpostag
                    "_",                # feat
                    str(int(head)+ 1),  # head
                    deprel,             # deprel
                    "_",
                    "_"]
            print "\t".join(output)
