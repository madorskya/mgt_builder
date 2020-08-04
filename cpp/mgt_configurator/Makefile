LOCINC     = -I.
OPTIM      = -O2
CCOPT      = -Wall $(OPTIM) $(LOCINC)
CCFILES   := drp_unit.cpp fpga.cpp main.cpp
OBJECTS    = $(CCFILES:.cpp=.o)
CC         = g++
CL         = g++

## rules	
#%.o: %.cpp
#	$(CC) $(CCOPT) -c $< -o $*.o

#%: %.o
#	$(CL) $(CCOPT) -o $@ $^

all: mgt_configurator

## dependencies
mgt_configurator: main.o fpga.o drp_unit.o
	$(CL) $(CCOPT) -o mgt_configurator main.o fpga.o drp_unit.o

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


