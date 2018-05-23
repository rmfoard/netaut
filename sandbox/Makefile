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
LIBS += -lrt -ljsoncpp -lpthread
# End global configuration

## Program-specific configuration
LEVELDB_P = /home/richard/leveldb
LEVELDB_LIB = $(LEVELDB_P)/build/libleveldb.a

DEMOS = \
	demo \
	\

all: $(DEMOS)

demo: demo.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

jtoy: jtoy.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(LDFLAGS) $(LIBS)

ldtoy: ldtoy.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(LDFLAGS) $(LIBS) $(LEVELDB_LIB)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

machine: machine.cpp rule.o $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp rule.o $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

rule: rule.o

rule.o: rule.cpp rule.h

clean:
	rm -f *.o $(DEMOS) rule machine
	rm -rf Debug Release
	rm -rf *.Err
