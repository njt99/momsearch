COLOROBJS= color.o AComplex1Jet.o GL2ACJ.o GL2C.o Box.o ImpossibleRelations.o TestCollection.o TestSearch.o Params.o
EVALOBJS= Evaluate.o AComplex1Jet.o GL2ACJ.o GL2C.o Box.o Params.o
REFINEOBJS= MomRefine.o BallSearch.o TestCollection.o Box.o ImpossibleRelations.o ACJ.o Params.o SL2C.o SL2ACJ.o Complex.o roundoff.o CanonicalName.o QuasiRelators.o
TREECATOBJS= treecat.o
CPLUSPLUS= c++
CC= g++

#COPTS=-g -Wall
COPTS=-O4

.cpp.o:
	$(CPLUSPLUS) $(COPTS) $(CCFLAGS) -c ${@:o=cpp}

.C.o:
	$(CC) $(COPTS) $(CCFLAGS) -c ${@:o=C}

color: $(COLOROBJS)
	$(CPLUSPLUS) $(COPTS) $(COLOROBJS) $(LIBS) -lm -o color
	
refine: $(REFINEOBJS)
	$(CPLUSPLUS) $(COPTS) $(REFINEOBJS) $(LIBS) -lm -o refine

evaluate: $(EVALOBJS)
	$(CPLUSPLUS) $(COPTS) $(EVALOBJS) $(LIBS) -lm -o evaluate

treecat: $(TREECATOBJS)
	$(CPLUSPLUS) $(COPTS) $(TREECATOBJS) -o treecat
