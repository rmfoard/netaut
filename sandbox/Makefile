#
# Makefile for non-Microsoft compilers
#	tested only on Linux

#include ../Makefile.config

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
CXXFLAGS += -std=c++98 -Wall
CXXFLAGS += -O3 -DNDEBUG -fopenmp
# turn on for crash debugging, get symbols with <prog> 2>&1 | c++filt
CXXFLAGS += -g -rdynamic
CXXFLAGS += -ggdb
# turn on for OpenMP
CXXOPENMP = 
LDFLAGS +=
LIBS += -lrt

## Main application file
DEMOS = \
	demo-gio \
	\

all: $(DEMOS)

# COMPILE
demo-%: demo-%.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

clean:
	rm -f *.o $(DEMOS) *.exe
	rm -rf Debug Release
	rm -rf *.Err demo*.dat

