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
CXXFLAGS += -Wall -Wno-delete-non-virtual-dtor # Suppress voluminous SNAP compiler warnings.
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

explorer: explorer.o machine.o machine2D.o machineR.o filter.o rule.o $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.o machine.o machine2D.o machineR.o filter.o rule.o $(CSNAP)/Snap.o $(LDFLAGS) $(LIBS)

explorer.o: explorer.cpp machine.h machine2D.h machineR.h rule.h
	$(CC) $(CXXFLAGS) -c -o $@ explorer.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

machine.o: machine.cpp rule.h machine.h
	$(CC) $(CXXFLAGS) -c -o $@ machine.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

machine2D.o: machine2D.cpp rule.h machine.h machine2D.h
	$(CC) $(CXXFLAGS) -c -o $@ machine2D.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

machineR.o: machineR.cpp rule.h machine.h machineR.h
	$(CC) $(CXXFLAGS) -c -o $@ machineR.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

runner.h: machine.h

runner.o: runner.cpp runner.h netaut.h machine.h machine2D.h rule.h
	$(CC) $(CXXFLAGS) -c -o $@ runner.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

chromosome.o: chromosome.cpp chromosome.h netaut.h runner.h
	$(CC) $(CXXFLAGS) -c -o $@ chromosome.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

pool.o: pool.cpp pool.h chromosome.h netaut.h runner.h
	$(CC) $(CXXFLAGS) -c -o $@ pool.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

picklist.o: picklist.cpp picklist.h pool.h chromosome.h netaut.h runner.h
	$(CC) $(CXXFLAGS) -c -o $@ picklist.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

searcher.o: searcher.cpp runner.h chromosome.h picklist.h pool.h
	$(CC) $(CXXFLAGS) -c -o $@ searcher.cpp -I$(CSNAP) -I$(CGLIB) -I$(CBOOST)

searcher: searcher.o runner.o chromosome.o pool.o picklist.o machine.o machine2D.o machineR.o filter.o rule.o $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.o runner.o chromosome.o pool.o picklist.o machine.o machine2D.o machineR.o filter.o rule.o $(CSNAP)/Snap.o $(LDFLAGS) $(LIBS)

toy.o: toy.cpp
	$(CC) $(CXXFLAGS) -c -o $@ toy.cpp

toy: toy.o
	$(CC) $(CXXFLAGS) -o $@ $@.o $(LDFLAGS) $(LIBS)

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
