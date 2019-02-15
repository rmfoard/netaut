#
#	Global configuration for SNAP makefiles
#
GLIB = glib-core
SNAP = snap-core
GLIBADV = glib-adv
SNAPADV = snap-adv
SNAPEXP = snap-exp

CGLIB = ../../snap/$(GLIB)
CSNAP = ../../snap/$(SNAP)
CPQ = ../../libpqxx/include
CBOOST = ../../boost_1_67_0

UNAME := $(shell uname)

# Linux flags
CC = g++
CXXFLAGS += -std=c++11 -Wall
CXXFLAGS += -O0 -fopenmp
# turn on for crash debugging, get symbols with <prog> 2>&1 | c++filt
CXXFLAGS += -g -rdynamic
CXXFLAGS += -ggdb
CXXFLAGS += -fPIC
# turn on for OpenMP
CXXOPENMP = 
LDFLAGS +=
LIBS += -lrt -ljsoncpp -lpthread -lpqxx -lpq
# End global configuration

## Program-specific configuration

all: explorer

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

explorer: explorer.o machine.o machine.h machine2D.o machineR.o filter.o rule.o $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.o machine.o machine2D.o machineR.o filter.o rule.o $(CSNAP)/Snap.o $(LDFLAGS) $(LIBS)

explorer.o: explorer.cpp machine.h machine2D.h machineR.h rule.h
	$(CC) $(CXXFLAGS) -c -o $@ explorer.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

machine.o: machine.cpp rule.h machine.h
	$(CC) $(CXXFLAGS) -c -o $@ machine.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

machine2D.o: machine2D.cpp rule.h machine.h machine2D.h
	$(CC) $(CXXFLAGS) -c -o $@ machine2D.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

machineR.o: machineR.cpp rule.h machine.h machineR.h
	$(CC) $(CXXFLAGS) -c -o $@ machineR.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

# The following have not been audited/overhauled.
randrule:	randrule.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(LDFLAGS) $(LIBS)

rulesource:	rulesource.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(LDFLAGS) $(LIBS)

txrule:	txrule.o rule.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp rule.o $(LDFLAGS) $(LIBS)

rule.o: rule.cpp rule.h rulemask.h

clean:
	rm -f *.o $(DEMOS) rule
	rm -rf Debug Release
	rm -rf *.Err
