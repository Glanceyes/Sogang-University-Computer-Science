#!/usr/bin/env python

# Use the sys module
import sys
import string

# 'file' in this case is STDIN
for line in sys.stdin:
    record = line.split(',')
    key = int(record[0])
    value = float(record[1])
    print('%d\t%s'%(key, value))
