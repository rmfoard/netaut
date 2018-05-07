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
LIBS += -lrt
# End global configuration

## Program-specific configuration
DEMOS = \
	demo \
	\

all: $(DEMOS)

demo: demo.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

machine: machine.cpp rules.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp rules.o $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o $(DEMOS) rules machine
	rm -rf Debug Release
	rm -rf *.Err

