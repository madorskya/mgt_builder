LOCINC     = -I.
OPTIM      = -O2
CCOPT      = -Wall $(OPTIM) $(LOCINC)
CC         = g++
CL         = g++

all: reg_configurator

reg_configurator: main.o drp_unit.o
	$(CL) $(CCOPT) -o reg_configurator main.o drp_unit.o

drp_unit.o : drp_unit.cpp drp_unit.h
	$(CC) $(CCOPT) -c -o drp_unit.o drp_unit.cpp

main.o : main.cpp drp_unit.h
	$(CC) $(CCOPT) -c -o main.o main.cpp

clean :
	rm -f *~
	rm -f *.o
	rm -f reg_configurator


