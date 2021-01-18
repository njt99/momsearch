REFINEOBJS= MomRefine.o BallSearch.o slopes.o TestCollection.o Box.o ImpossibleRelations.o ACJ.o Params.o SL2C.o SL2ACJ.o Complex.o roundoff.o CanonicalName.o QuasiRelators.o
TREECATOBJS= treecat.o
STREECATOBJS= simple_tree_cat.o
TESTOBJS= test.o BallSearch.o slopes.o TestCollection.o Box.o ImpossibleRelations.o ACJ.o Params.o SL2C.o SL2ACJ.o Complex.o roundoff.o CanonicalName.o QuasiRelators.o
#CPLUSPLUS= icpc
#CC= icpc
CPLUSPLUS= g++
CC= g++

#COPTS=-O3 -std=c++17 -stdlib=libc++
#COPTS=-xHost -O3 -std=c++17
#COPTS=-g -pg -Wall -std=c++1y
COPTS=-O3 -std=c++1y

.cpp.o:
	$(CPLUSPLUS) $(COPTS) $(CCFLAGS) -c ${@:o=cpp}

.C.o:
	$(CC) $(COPTS) $(CCFLAGS) -c ${@:o=C}

debug: $(REFINEOBJS)
	$(CPLUSPLUS) $(COPTS) $(REFINEOBJS) $(LIBS) -lm -o ../bin/refine_debug

refine: $(REFINEOBJS)
	$(CPLUSPLUS) $(COPTS) $(REFINEOBJS) $(LIBS) -lm -o ../bin/refine_slopes

top: $(REFINEOBJS)
	$(CPLUSPLUS) $(COPTS) $(REFINEOBJS) $(LIBS) -lm -o ../bin/refine_slopes_top

mid: $(REFINEOBJS)
	$(CPLUSPLUS) $(COPTS) $(REFINEOBJS) $(LIBS) -lm -o ../bin/refine_slopes_mid

mt: $(REFINEOBJS)
	$(CPLUSPLUS) $(COPTS) $(REFINEOBJS) $(LIBS) -lm -o ../bin/refine_slopes_mt

test: $(TESTOBJS)
	$(CPLUSPLUS) $(COPTS) $(TESTOBJS) $(LIBS) -lm -o test

treecat: $(TREECATOBJS)
	$(CPLUSPLUS) $(COPTS) $(TREECATOBJS) -o ../bin/treecat_debug

simple_tree_cat: $(STREECATOBJS)
	$(CPLUSPLUS) $(COPTS) $(STREECATOBJS) -o ../bin/simple_tree_cat
