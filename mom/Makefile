COLOROBJS= color.o AComplex1Jet.o GL2ACJ.o GL2C.o
CPLUSPLUS= g++
CC= g++
COPTS=-g -Wall

.C.o:
	$(CPLUSPLUS) $(COPTS) $(CCFLAGS) -c ${@:o=C}

color: $(COLOROBJS)
	$(CPLUSPLUS) $(COPTS) $(COLOROBJS) $(LIBS) -lm -o color
	
	
