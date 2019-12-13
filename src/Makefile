REFINEOBJS= MomRefine.o BallSearch.o TestCollection.o Box.o ImpossibleRelations.o ACJ.o Params.o SL2C.o SL2ACJ.o Complex.o roundoff.o CanonicalName.o QuasiRelators.o
SLOPEOBJS= Box.o ACJ.o Params.o SL2C.o SL2ACJ.o Complex.o roundoff.o Slopes.o
TREECATOBJS= treecat.o
STREECATOBJS= simple_tree_cat.o
#CPLUSPLUS= icpc
#CC= icpc
CPLUSPLUS= g++
CC= g++

#COPTS=-O3 -std=c++17 -stdlib=libc++
#COPTS=-xHost -O3 -std=c++17
#COPTS=-g3 -Wall -std=c++17
COPTS=-O3 -std=c++17

.cpp.o:
	$(CPLUSPLUS) $(COPTS) $(CCFLAGS) -c ${@:o=cpp}

.C.o:
	$(CC) $(COPTS) $(CCFLAGS) -c ${@:o=C}

slopes: $(SLOPEOBJS)
	$(CPLUSPLUS) $(COPTS) $(SLOPEOBJS) $(LIBS) -lm -o ../bin/slopes

refine: $(REFINEOBJS)
	$(CPLUSPLUS) $(COPTS) $(REFINEOBJS) $(LIBS) -lm -o ../bin/refine_debug

treecat: $(TREECATOBJS)
	$(CPLUSPLUS) $(COPTS) $(TREECATOBJS) -o ../bin/treecat

simple_tree_cat: $(STREECATOBJS)
	$(CPLUSPLUS) $(COPTS) $(STREECATOBJS) -o ../bin/simple_tree_cat
