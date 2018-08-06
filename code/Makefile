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

UNAME := $(shell uname)

# Linux flags
CC = g++
CXXFLAGS += -std=c++11 -Wall
CXXFLAGS += -O0 -fopenmp
# turn on for crash debugging, get symbols with <prog> 2>&1 | c++filt
CXXFLAGS += -g -rdynamic
CXXFLAGS += -ggdb
# turn on for OpenMP
CXXOPENMP = 
LDFLAGS +=
LIBS += -lrt -ljsoncpp -lpthread -lpqxx -lpq
# End global configuration

## Program-specific configuration

DEMOS = \
	demo \
	\

all: $(DEMOS)

demo: demo.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

ldtoy: ldtoy.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(LDFLAGS) $(LIBS)

insUentropy: insUentropy.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp -I$(CPQ) $(LDFLAGS) $(LIBS)

normEntropy: normEntropy.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp -I$(CPQ) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

explorer: explorer.cpp machine.cpp machine.h machine2D.cpp machine2D.h rule.h filter.o rule.o $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp machine.cpp machine2D.cpp filter.o rule.o $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

randrule:	randrule.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(LDFLAGS) $(LIBS)

rulesource:	rulesource.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(LDFLAGS) $(LIBS)

toy:	toy.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(LDFLAGS) $(LIBS)

txrule:	txrule.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp rule.o $(LDFLAGS) $(LIBS)

rule: rule.o

rule.o: rule.cpp rule.h

clean:
	rm -f *.o $(DEMOS) rule
	rm -rf Debug Release
	rm -rf *.Err
