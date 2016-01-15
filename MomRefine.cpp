/*
 *  MomRefine.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 10/10/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */

#include <getopt.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#include <stdlib.h>
#include <string.h>

#include "Box.h"
#include "TestCollection.h"
#include "BallSearch.h"
#include "QuasiRelators.h"

using namespace std;

struct Options {
	Options() :
        boxName(""), // Binary representation of box
        wordsFile("./current_refine_run"), // Previously generated words
		powersFile("null"), // Output from power parabolic.pl
		momFile("null"), // TODO: Find what generates
		parameterizedFile("null"), // TODO: Find what generates
		maxDepth(18), // Maximum depth for a file
        truncateDepth(6), 
        inventDepth(12),
        maxSize(1000000),
		improveTree(false),
		ballSearchDepth(-1),
		fillHoles(false),
		maxWordLength(20) {}
	const char* boxName;
	const char* wordsFile;
	const char* powersFile;
	const char* momFile;
	const char* parameterizedFile;
	int maxDepth;
	int truncateDepth;
	int inventDepth;
	int maxSize;
	bool improveTree;
	int ballSearchDepth;
	bool fillHoles;
	int maxWordLength;
};

Options g_options;
TestCollection g_tests;
typedef vector< vector< bool > > TestHistory;
set<string> g_momVarieties;
set<string> g_parameterizedVarieties;
static int g_boxesVisited = 0;

struct PartialTree {
	PartialTree() :lChild(NULL), rChild(NULL), testIndex(-1), box(NULL) {}
	PartialTree *lChild;
	PartialTree *rChild;
	int testIndex;
    NamedBox *box;
};

// Consume tree from stdin. The tree must be
// provided in pre-order depth-first traversal.
PartialTree readTree()
{
	PartialTree t;
	char buf[1000];
	if (!fgets(buf, sizeof(buf), stdin)) {
		fprintf(stderr, "unexpected EOF\n");
		exit(1);
	}
	int n = strlen(buf);
	if (buf[n-1] == '\n')
		buf[n-1] = '\0';
	if (buf[0] == 'X') {
		t.lChild = new PartialTree(readTree());
		t.rChild = new PartialTree(readTree());
	} else if (strstr(buf, "HOLE") != NULL) {
		t.testIndex = -2;
	} else {
		if (isdigit(buf[0])) {
			t.testIndex = atoi(buf);
		} else {
            // Add word as eliminator to test collection
			t.testIndex = g_tests.add(buf);
		}
	}
	return t;
}

void truncateTree(PartialTree& t)
{
	if (t.lChild) {
		truncateTree(*t.lChild);
		delete t.lChild;
		t.lChild = 0;
	}
	if (t.rChild) {
		truncateTree(*t.rChild);
		delete t.rChild;
		t.rChild = 0;
	}
}

int treeSize(PartialTree& t) {
	int size = 1;
	if (t.lChild)
		size += treeSize(*t.lChild);
	if (t.rChild)
		size += treeSize(*t.rChild);
	return size;
}


extern string g_testCollectionFullWord;
extern int g_xLattice;
extern int g_yLattice;
extern double g_maximumArea;

// Provides the word for a testIndex while
// multplied by g_xLattice copies of m and g_yLattice
// copies of n on the left. The front abelian subword is
// rewriteen with all m's first and n's second.
string relatorName(int testIndex)
{
    // The global lattice counts (# of m and n's) should be set
    // by TestCollection.evaluate(). TODO Check when these get reset
	int xLattice = g_xLattice;
	int yLattice = g_yLattice;
	const char *wordName = g_tests.getName(testIndex);
	bool done = false;
	for (;;) {
		switch(*wordName) {
			case 'm': ++xLattice; break;
			case 'M': --xLattice; break;
			case 'n': ++yLattice; break;
			case 'N': --yLattice; break;
			default: done = true; break;
		}
		if (done) break;
		++wordName;
	}
	char buf[10000];
	char *bp = buf;
	for (int i = 0; i < xLattice; ++i)
		*bp++ = 'm';
	for (int i = 0; i < -xLattice; ++i)
		*bp++ = 'M';
	for (int i = 0; i < yLattice; ++i)
		*bp++ = 'n';
	for (int i = 0; i < -yLattice; ++i)
		*bp++ = 'N';
	strcpy(bp, wordName);
	return buf;
}

// For test type n = 6 varfies if valid identity,
// mom variety, or parametrized variety. Otherwise, tests if 
// n == 1, 3, 4, or 5.
bool isEliminated(int testIndex, int n, NamedBox& box) {
	if (n == 6) {
		string w = box.qr.getName(relatorName(testIndex));
		if (!g_tests.validIdentity(w, box))
			return true;
		if (g_momVarieties.find(w) != g_momVarieties.end()) {
			fprintf(stderr, "mom variety %s(%s)\n", w.c_str(), box.name.c_str());
			return true;
		}
		if (g_parameterizedVarieties.find(w) != g_parameterizedVarieties.end()) {
			fprintf(stderr, "parameterized variety %s(%s)\n", w.c_str(), box.name.c_str());
			return true;
		}
	}

	return (n == 1 || n == 3 || n == 4 || n == 5);
}

extern double g_latticeArea;
bool refineRecursive(NamedBox box, PartialTree& t, int depth, TestHistory& history, vector< NamedBox >& place, int newDepth, int& searchedDepth)
{
	//fprintf(stderr, "rr: %s depth %d placeSize %lu\n", box.name.c_str(), depth, place.size());
	place.push_back(box);
    t.box = &box; 
	int oldTestIndex = t.testIndex;
	if (t.testIndex >= 0) {
        int result = g_tests.evaluateBox(t.testIndex, box);
        if (isEliminated(t.testIndex, result, box)) {
//            fprintf(stderr, "Eliminated %s with test %s with result %d\n", box.name.c_str(), g_tests.getName(t.testIndex), result);
            return true;
        } else {
            fprintf(stderr, "FAILED to eliminate %s with test %s with result %d\n", box.name.c_str(), g_tests.getName(t.testIndex), result);
        }
    }
	if (t.testIndex == -2 && !g_options.fillHoles)
		return true;
	if (g_options.ballSearchDepth >= 0 && (g_options.improveTree || !t.lChild)) {
		while (depth - searchedDepth > g_options.ballSearchDepth) {
			NamedBox& searchPlace = place[++searchedDepth];
			vector<string> searchWords = findWords( searchPlace.center(), vector<string>(), -200, g_options.maxWordLength, box.qr.wordClasses());
			fprintf(stderr, "search (%s) found %s(%s)\n",
				searchPlace.qr.desc().c_str(), searchWords.back().c_str(), searchPlace.name.c_str());
			g_tests.add(searchWords.back());
			history.resize(g_tests.size());
		}
	}
	
	if (g_options.improveTree || !t.lChild) {
		map<string, int> hackIndex;
		for (int i = 0; i < g_tests.size(); ++i) {
			vector<bool>& th = history[i];
			while (th.size() <= depth && (th.size() < depth-6 || th.empty() || th.back())) {
				bool result = g_tests.evaluateCenter(i, place[th.size()]);
				//fprintf(stderr, "test %s(%s) = %s\n", g_tests.getName(i), place[th.size()].name.c_str(), result ? "true" : "false");
				th.push_back(result);
			}
			if (th.back()) {
				int result = g_tests.evaluateBox(i, box);
				//fprintf(stderr, "evaluate %s(%s) = %d\n", g_tests.getName(i), box.name.c_str(), result);
				if (result == 1 || result == 3 || result == 4 || result == 5) {
					if (result == 3)
						fprintf(stderr, "impossible identity %s(%s)\n", g_tests.getName(i), box.name.c_str());
					if (result == 4)
						fprintf(stderr, "impossible lattice %s(%s)\n", g_tests.getName(i), box.name.c_str());
					if (result == 5)
						fprintf(stderr, "impossible power %s(%s)\n", g_testCollectionFullWord.c_str(), box.name.c_str());
					t.testIndex = i;
					return true;
				} else if (result == 6) {
					string w = box.qr.getName(relatorName(i));
					hackIndex[w] = i;
					if (g_momVarieties.find(w) != g_momVarieties.end()) {
						t.testIndex = i;
						fprintf(stderr, "mom variety %s(%s)\n", w.c_str(), box.name.c_str());
						return true;
					}
					if (g_parameterizedVarieties.find(w) != g_parameterizedVarieties.end()) {
						t.testIndex = i;
						fprintf(stderr, "parameterized variety %s(%s)\n", w.c_str(), box.name.c_str());
						return true;
					}
				}
			}
		}
		vector<string> quasiRelators = box.qr.wordClasses();
		for (vector<string>::iterator it = quasiRelators.begin(); it != quasiRelators.end(); ++it) {
			if (!g_tests.validIdentity(*it, box)) {
				fprintf(stderr, "invalid identity %s(%s)\n", it->c_str(), box.name.c_str());
				t.testIndex = hackIndex[*it];
				return true;
			}
		}
	}
	
	t.testIndex = -1;
	
	if (!t.lChild) {
		if (depth >= g_options.maxDepth || ++g_boxesVisited >= g_options.maxSize || ++newDepth > g_options.inventDepth) {
			// fprintf(stderr, "HOLE %s (%s)\n", box.name.c_str(), box.qr.desc().c_str());
	        Params<Complex> params = box.center();
	        Params<Complex> nearest = box.nearest();
			Params<AComplex1Jet> cover(box.cover());
			double absLS = minabs(cover.loxodromicSqrt);
			double area = absLS * absLS * nearest.lattice.imag();
			fprintf(stderr, "HOLE %s has min area: %f center lat: %f + I %f lox: %f + I %f par: %f + I %f size: %.4e (%s)\n", box.name.c_str(), area, params.lattice.real(), params.lattice.imag(), params.loxodromicSqrt.real(), params.loxodromicSqrt.imag(), params.parabolic.real(),params.parabolic.imag(), box.size(), box.qr.desc().c_str());
			return false;
		}
		t.lChild = new PartialTree();
		t.rChild = new PartialTree();
	}
	bool isComplete = true;
	isComplete = refineRecursive(box.child(0), *t.lChild, depth+1, history, place, newDepth, searchedDepth) && isComplete;
	if (place.size() > depth+1)
		place.resize(depth+1);
	for (int i = 0; i < g_tests.size(); ++i) {
		if (history[i].size() > depth)
			history[i].resize(depth);
	}
	if (searchedDepth > depth)
		searchedDepth = depth;
	if (isComplete || depth < g_options.truncateDepth)
		isComplete = refineRecursive(box.child(1), *t.rChild, depth+1, history, place, newDepth, searchedDepth) && isComplete;
	if (oldTestIndex >= 0 && t.testIndex != oldTestIndex) {
		fprintf(stderr, "invalid box %s(%s) %d %s\n", g_tests.getName(oldTestIndex), box.name.c_str(),
			treeSize(t), isComplete ? "Patched" : "Unpatched");
	}
	if (!isComplete && depth >= g_options.truncateDepth) {
		truncateTree(t);
	}
	return isComplete;
}

void refineTree(NamedBox box, PartialTree& t)
{
	TestHistory history(g_tests.size());
	vector<NamedBox> place;
	int searchedDepth = 0;
	refineRecursive(box, t, 0, history, place, 0, searchedDepth);
}

void printTree(PartialTree& t)
{
	if (t.testIndex >= 0) {
		printf("%s\n", g_tests.getName(t.testIndex));
	} else if (t.lChild && t.rChild) {
		printf("X\n");
		printTree(*t.lChild);
		printTree(*t.rChild);
	} else {
		printf("HOLE (%s)\n", t.box->qr.desc().c_str());
	}
}

const char* g_programName;


static struct option longOptions[] = {
	{"box",	required_argument, NULL, 'b' },
	{"words", required_argument, NULL, 'w' },
	{"powers", required_argument, NULL, 'p'},
	{"mom", required_argument, NULL, 'M'},
	{"parameterized", required_argument, NULL, 'P'},
	{"maxDepth", required_argument, NULL, 'm' },
	{"inventDepth", required_argument, NULL, 'i' },
	{"improveTree", no_argument, NULL, 'I'},
	{"truncateDepth", required_argument, NULL, 't' },
	{"maxSize", required_argument, NULL, 's' },
	{"ballSearchDepth", required_argument, NULL, 'B'},
	{"fillHoles", no_argument, NULL, 'f'},
	{"maxArea", required_argument, NULL, 'a'},
	{NULL, 0, NULL, 0}
};

static char optStr[1000] = "";
void setOptStr() {
	char* osp = optStr;
	for (int i = 0; longOptions[i].name; ++i) {
		*osp++ = longOptions[i].val;
		if (longOptions[i].has_arg != no_argument)
			*osp++ = ':';
	}
	*osp = '\0';
}

void usage()
{
	const char* longUsage = "\
       --box <box_id>\n\
       		Box ID for the root of the input and output trees.\n\
		May include characters not in [01], which are ignored.\n\
\n\
Options controlling which relators to use:\n\
	[ --words	<words_file> ]\n\
		File containing starting list of words to try.\n\
	[ --ballSearchDepth <n> ]\n\
		Perform a search for relators when visiting a node at least n levels deep.\n\
\n\
Options controlling which relators eliminate boxes:\n\
	[ --powers <powers_file> ]\n\
		File containing impossible relator definitions.\n\
		See ImpossibleRelators::load(...)\n\
	[ --mom <mom_varieties_file> ]\n\
	[ --parameterized <parameterized_varieties_file ]\n\
		Files containg varieties which the user wishes to ignore.\n\
		The ony functional difference is that they produce different log messages.\n\
	[ --maxArea <v> ]\n\
		Eliminate boxes which have area > maxArea.\n\
\n\
Options controlling tree manipulation:\n\
	[ --maxDepth <n> ]\n\
		Don't descend more than n levels deeper than the root box.\n\
	[ --inventDepth <n> ]\n\
		Don't descend more than n levels deeper than the terminal node of the input tree.\n\
	[ --maxSize <n> ]\n\
		Don't allow the output tree to have more than n nodes.\n\
	[ --truncateDepth <n> ]\n\
		Don't emit holes more than n levels deeper than the root node.\n\
		Instead, replace the subtree-with-holes with a single hole.\n\
	[ --improveTree ]\n\
		If set, attempt to directly eliminate internal nodes of the input tree.\n\
	[ --fillHoles ]\n\
		If set, attempt to patch holes in the input tree.\n\
";
	fprintf(stderr, "Usage: %s %s\n\n%s", g_programName, optStr, longUsage);
}

void loadWords(set<string>& s, const char* fileName)
{
	FILE* fp = fopen(fileName, "r");
	char buf[10000];
	while (fp && fgets(buf, sizeof(buf), fp)) {
		int n = -1 + strlen(buf);
		if (buf[n] == '\n')
			buf[n] = '\0';
		s.insert(buf);
	}
}

int main(int argc, char** argv)
{
	setOptStr();
	if (argc < 2) {
		usage();
		exit(1);
	}
	int ch;
	while ((ch = getopt_long(argc, argv, optStr, longOptions, NULL)) != -1) {
		switch(ch) {
		case 'b': g_options.boxName = optarg; break;
		case 'w': g_options.wordsFile = optarg; break;
		case 'p': g_options.powersFile = optarg; break;
		case 'M': g_options.momFile = optarg; break;
		case 'P': g_options.parameterizedFile = optarg; break;
		case 'm': g_options.maxDepth = atoi(optarg); break;
		case 'i': g_options.inventDepth = atoi(optarg); break;
		case 'I': g_options.improveTree = true; break;
		case 't': g_options.truncateDepth = atoi(optarg); break;
		case 's': g_options.maxSize = atoi(optarg); break;
		case 'B': g_options.ballSearchDepth = atoi(optarg); break;
		case 'f': g_options.fillHoles = true; break;
		case 'a': g_maximumArea = atof(optarg); break;
		}
	}
	
	NamedBox box;
	for (const char* boxP = g_options.boxName; *boxP; ++boxP) {
		if (*boxP == '0') {
			box = box.child(0);
		} else if (*boxP == '1') {
			box = box.child(1);
		}
	}
	
	g_tests.load(g_options.wordsFile);
	g_tests.loadImpossibleRelations(g_options.powersFile);
	loadWords(g_momVarieties, g_options.momFile);
	loadWords(g_parameterizedVarieties, g_options.parameterizedFile);
	
	PartialTree t = readTree();
	refineTree(box, t);
	printTree(t);
	fprintf(stderr, "%d nodes added\n", g_boxesVisited);
}
