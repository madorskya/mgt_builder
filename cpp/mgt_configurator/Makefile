HARDWARE   = -DMTF7
LN_LOC     = ../../../linenoise_cpp/
LD_LOC     = /opt/cactus/include/
LOCINC     = -I. -I$(LN_LOC)
LDINC      = -I$(LD_LOC)
OPTIM      = -O2
LDLIBS     = -L/opt/cactus/lib/
OPT        = -Wall $(OPTIM) $(LOCINC) $(HARDWARE)
LDOPT      = -Wall $(OPTIM) $(LOCINC) $(LDINC) $(HARDWARE) $(LDLIBS)
LIB        = -lreadline -lncurses -lboost_regex -lboost_filesystem -lboost_system -lboost_serialization -std=c++11
CC         = g++
CL         = g++

ifeq ($(HARDWARE),-DMTF7)
  CCOPT=$(LDOPT)
else
  CCOPT=$(OPT)
endif

all: mgt_configurator
ifeq ($(HARDWARE),-DMTF7)
        export LD_LIBRARY_PATH=/opt/cactus/lib
endif

mgt_configurator: main.o fpga.o drp_unit.o linenoise.o
	$(CL) $(CCOPT) $(LIB) -o mgt_configurator main.o fpga.o drp_unit.o linenoise.o

linenoise.o: $(LN_LOC)/linenoise.cpp $(LN_LOC)/linenoise.h
	$(CC) $(CCOPT) -c -o linenoise.o $(LN_LOC)/linenoise.cpp

drp_unit.o : drp_unit.cpp drp_unit.h
	$(CC) $(CCOPT) -c -o drp_unit.o drp_unit.cpp

fpga.o : fpga.cpp fpga.h drp_unit.h
	$(CC) $(CCOPT) -c -o fpga.o fpga.cpp

main.o : main.cpp fpga.h drp_unit.h $(LN_LOC)/linenoise.h
	$(CC) $(CCOPT) -c -o main.o main.cpp

clean :
	rm -f *~
	rm -f *.o
	rm -f mgt_configurator
