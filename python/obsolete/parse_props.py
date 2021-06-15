#!/usr/bin/python

import sys
import os 

infile = 'gty_props.txt'
outfile = 'props.out'
nargs = len(sys.argv) 

if nargs > 1:
    infile = sys.argv[1]

def parse_props(fn):
    fout = open(outfile, "w") 
    fin = open(fn,"r")
    print("Parsing %s" % fn)
    for line in fin:
        props = line.split() 
        if (len(props) == 4 and "Property" not in props[0]):
           if (("." not in props[3]) and ("/" not in props[3])):
              print ("%s %s" % (props[0],props[3]))
              fout.write("%s %s\n" % (props[0],props[3]))
    fin.close()
    fout.close()
    print("Wrote parsed data to %s" % outfile)

parse_props(infile) 
