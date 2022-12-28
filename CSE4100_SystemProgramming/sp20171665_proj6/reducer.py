#!/usr/bin/env python

# import modules
from sys import stdin, stdout
import sys


current_key = 0
max_value = 0
key = None

for var in sys.stdin:
    line = var.strip()
    key, number = line.split('\t', 1)
    try:
        # For each word, pull the count(s) for the word
        #   from 'group' and create a total count
        key = int(key)
        number = float(number)
    except ValueError:
        # Count was not a number, so do nothing
        pass
    if current_key == key:
        if max_value < number:
            max_value = number
    else:
        print("%s,%s" %(current_key, max_value))
        current_key = key
        max_value = number
                                        
# Write to stdout        
print("%s,%s" %(current_key, max_value))
