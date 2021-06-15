#!/usr/bin/python

import sys
import os 

infile = 'gty_nets.txt'
outfile = 'nets.out'
nargs = len(sys.argv) 

if nargs > 1:
    infile = sys.argv[1]

def parse_nets(fn):
    fout = open(outfile, "w") 
    fin = open(fn,"r")
    print("Parsing %s" % fn)
    pin_mbits = ''
    net_mbits = "m'b"
    mbits = 0
    for line in fin:
        nets = line.split() 
        if (len(nets) == 2):
           if (('const0' in nets[1]) or ('const1' in nets[1])):

              pin = nets[0][nets[0].rfind('/')+1:]
              net = nets[1][nets[1].rfind('/')+1:]
              # print ("in-> %s %s" % (pin,net))

	      if ('[' not in pin):

		 net = net.replace('<const',"1'b").replace('>','')

                 # Print and reset found multi-bits net after switch
                 if (len(pin_mbits) > 0) and (mbits == 1):
                     print ("%s %s" % (pin_mbits,net_mbits))
                     fout.write("%s %s\n" % (pin_mbits,net_mbits))
		 pin_mbits = ''
                 net_mbits = "" 
		 mbits = 0
 
                 # Print one-bit net
                 print ("%s %s" % (pin,net))
                 fout.write("%s %s\n" % (pin,net))

              # Accumulate multi-bits
              if ('[' in pin):
                 if (pin_mbits != pin[:pin.rfind('[')]):
                     # Print and reset found multi-bits net after switch
                     if ((len(pin_mbits) > 0) and (mbits == 1)):
                        print ("%s %s" % (pin_mbits,net_mbits))
                        fout.write("%s %s\n" % (pin_mbits,net_mbits))
		     mbits = int(pin[pin.rfind('[')+1:pin.rfind(']')])+1
		     net_mbits = str(mbits)+"'b"

		 mbits = int(pin[pin.rfind('[')+1:pin.rfind(']')])+1    
		 pin_mbits = pin[:pin.rfind('[')]
                 net_mbits +=  net.replace('<const','').replace('>','')
                  

    # Print multi-bits after readout of last line in input file                
    if (len(pin_mbits) > 0):
        print ("%s %s" % (pin_mbits,net_mbits))
        fout.write("%s %s\n" % (pin_mbits,net_mbits))

    fin.close()
    fout.close()
    print("Wrote parsed data to %s" % outfile)

parse_nets(infile) 
