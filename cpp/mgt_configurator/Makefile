LOCINC     = -I.
OPTIM      = -O2
CCOPT      = -Wall $(OPTIM) $(LOCINC)
LIB        = -lreadline -lncurses
CC         = g++
CL         = g++

all: mgt_configurator

mgt_configurator: main.o fpga.o drp_unit.o
	$(CL) $(CCOPT) $(LIB) -o mgt_configurator main.o fpga.o drp_unit.o

drp_unit.o : drp_unit.cpp drp_unit.h
	$(CC) $(CCOPT) -c -o drp_unit.o drp_unit.cpp

fpga.o : fpga.cpp fpga.h drp_unit.h
	$(CC) $(CCOPT) -c -o fpga.o fpga.cpp

main.o : main.cpp fpga.h drp_unit.h
	$(CC) $(CCOPT) -c -o main.o main.cpp

clean :
	rm -f *~
	rm -f *.o
	rm -f mgt_configurator


