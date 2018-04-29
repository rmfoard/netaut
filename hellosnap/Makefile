#	implements:
#		all (default), clean
#

include ../Makefile.config

## Main application file
MAIN = hellosnap
DEPH = 
DEPCPP = 

# (following previously in Makefile.ex)
all: $(MAIN)

# COMPILE
$(MAIN): $(MAIN).cpp $(DEPH) $(DEPCPP) $(EXSNAP)/Snap.o 
	$(CC) $(CXXFLAGS) -o $(MAIN) $(MAIN).cpp $(DEPCPP) $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(EXSNAPADV) -I$(EXGLIB) -I$(EXSNAPEXP) $(LDFLAGS) $(LIBS)

$(EXSNAP)/Snap.o: 
	make -C $(EXSNAP)

clean:
	rm -f *.o  $(MAIN)
