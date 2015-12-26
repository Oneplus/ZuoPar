#!/usr/bin/env python

def encode(num):
    if num > 10:
        return 10
    elif num > 5:
        return 6
    else:
        return num

def encode1(num):
    if num >= 7:
        return 7
    elif num >= 3:
        return 3
    else:
        return num

token = ""
for i in xrange(256):
    if i%10==0:
        token += "    "
    token += ("%d," % encode(i))
    if (i+ 1) % 10 == 0:
        token += "\n"

token1 = ""
for i in xrange(256):
    if i%10==0:
        token1 += "    "
    token1 += ("%d," % encode1(i))
    if (i+ 1) % 10 == 0:
        token1 += "\n"

print '''#ifndef __ZUOPAR_UTILS_MATH_FAST_BINNED_H__
#define __ZUOPAR_UTILS_MATH_FAST_BINNED_H__

namespace ZuoPar {
namespace Math {
'''

print ("static int binned_1_2_3_4_5_6_10[] = {\n%s};\n" % token)

print ("static int binned_1_2_3_7[] = {\n%s};" % token1)

print '''
} //  end for math
} //  end for zuopar

#endif  //  end for __ZUOPAR_UTILS_MATH_FAST_BINNED_H__'''
