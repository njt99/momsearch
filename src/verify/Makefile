include Makefile.preamble
COMMON= ACJ.o Box.o Params.o SL2ACJ.o Complex.o roundoff.o
CPLUSPLUS= g++
CC=gcc

.SUFFIXES: .o .cpp

.cpp.o:
	$(CPLUSPLUS) $(COPTS) $(CCFLAGS) -c ${@:o=cpp}

verify_cusp: verify_cusp.o $(COMMON)
	$(CPLUSPLUS) $(COPTS) verify_cusp.o $(COMMON) $(LIBS) -o verify_cusp

depend:
	makedepend -I/usr/local/lib/g++-include -f Makefile.depend *.cpp
include Makefile.depend

# never optimize here, since the timing of arithmetic is important
test_float.o: test_float.cpp roundoff.h
	$(CPLUSPLUS) -g -c test_float.cpp

test_float: test_float.o roundoff.o roundoff.h
	$(CC) $(COPTS) test_float.o roundoff.o -o test_float -lm

# DO NOT DELETE THIS LINE -- make depend depends on it.
