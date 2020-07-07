#!/usr/bin/python

import re
import sys

varieties = []

with open(sys.arvg[1],'r') as fp :
    for line in fp :
        var_match = re.match('V\((.*)\)', line)
        if var_match :
            varieties.append(var_match.group(1))

Q
