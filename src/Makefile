REFINEOBJS= MomRefine.o BallSearch.o slopes.o TestCollection.o Box.o ImpossibleRelations.o ACJ.o Params.o SL2C.o SL2ACJ.o Complex.o roundoff.o CanonicalName.o QuasiRelators.o
TREECATOBJS= treecat.o
STREECATOBJS= simple_tree_cat.o
#CPLUSPLUS= icpc
#CC= icpc
CPLUSPLUS= g++
CC= g++

#COPTS=-O3 -std=c++11 -stdlib=libc++
#COPTS=-xHost -O3 -std=c++11
#COPTS=-g3 -Wall -std=c++11
#COPTS=-O3 -pg -std=c++11
COPTS=-O3 -std=c++11

.cpp.o:
	$(CPLUSPLUS) $(COPTS) $(CCFLAGS) -c ${@:o=cpp}

.C.o:
	$(CC) $(COPTS) $(CCFLAGS) -c ${@:o=C}

refine: $(REFINEOBJS)
	$(CPLUSPLUS) $(COPTS) $(REFINEOBJS) $(LIBS) -lm -o ../bin/refine_slopes

treecat: $(TREECATOBJS)
	$(CPLUSPLUS) $(COPTS) $(TREECATOBJS) -o ../bin/treecat_debug

simple_tree_cat: $(STREECATOBJS)
	$(CPLUSPLUS) $(COPTS) $(STREECATOBJS) -o ../bin/simple_tree_cat
