/*
 *  MomRefine.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 10/10/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */

#include "Box.h"
#include <getopt.h>
#include <stdio.h>
#include <vector>
#include <map>
#include "TestCollection.h"
#include "BallSearch.h"
#include "FindMomManifold.h"
#include "QuasiRelators.h"

using namespace std;

struct Options {
	Options() :boxName(""), wordsFile("/Users/njt/projects/mom/allWords"),
		powersFile("/Users/njt/projects/mom/wordpowers.out"),
		momsFile("/Users/njt/projects/mom/relatorMoms"),
		gluingsFile("/Users/njt/projects/mom/gluings"),
		maxDepth(18), truncateDepth(6), inventDepth(12), maxSize(1000000),
		improveTree(false),
		ballSearchDepth(-1) {}
	const char* boxName;
	const char* wordsFile;
	const char* powersFile;
	const char* momsFile;
	const char* gluingsFile;
	int maxDepth;
	int truncateDepth;
	int inventDepth;
	int maxSize;
	bool improveTree;
	int ballSearchDepth;
};

Options g_options;
TestCollection g_tests;

static int g_boxesVisited = 0;
struct PartialTree {
	PartialTree() :lChild(0), rChild(0), testIndex(-1) {}
	PartialTree *lChild;
	PartialTree *rChild;
	int testIndex;
};

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
	} else if (!strcmp(buf, "HOLE")) {
	} else {
		if (isdigit(buf[0])) {
			t.testIndex = atoi(buf);
		} else {
			t.testIndex = g_tests.add(buf);
		}
	}
	return t;
}

typedef vector< vector< bool > > TestHistory;

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

extern string g_testCollectionFullWord;

extern int g_xLattice;
extern int g_yLattice;

string relatorName(int testNumber)
{
	int xLattice = g_xLattice;
	int yLattice = g_yLattice;
	const char *wordName = g_tests.getName(testNumber);
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

extern double g_latticeArea;
bool refineRecursive(NamedBox box, PartialTree& t, int depth, TestHistory& history, vector< NamedBox >& place, int newDepth, int& searchedDepth)
{
//	fprintf(stderr, "rr: %s depth %d placeSize %d\n", box.name.c_str(), depth, place.size());
	place.push_back(box);
	if (t.testIndex >= 0) {
		int n = g_tests.evaluateBox(t.testIndex, box);
		if (n == 1 || n == 3 || n == 4 || n == 5 || n == 7)
			return true;
	}
	if (g_options.ballSearchDepth >= 0 && depth - searchedDepth > g_options.ballSearchDepth) {
		NamedBox& searchPlace = place[++searchedDepth];
		vector<string> searchWords = findWords( searchPlace.center(), vector<string>(), 1, 20, searchPlace.qr.relatorWords());
		fprintf(stderr, "search found %s(%s)\n",
			searchWords.back().c_str(), searchPlace.name.c_str());
		g_tests.add(searchWords.back());
		history.resize(g_tests.size());
	}
	
	if (g_options.improveTree || !t.lChild) {
		for (int i = 0; i < g_tests.size(); ++i) {
			vector<bool>& th = history[i];
			while (th.size() <= depth && (th.size() < depth-6 || th.empty() || th.back())) {
				bool result = g_tests.evaluateCenter(i, place[th.size()]);
//				fprintf(stderr, "test %s(%s) = %s\n", g_tests.getName(i), place[th.size()].name.c_str(),
//					result ? "true" : "false");
				th.push_back(result);
			}
			if (th.back()) {
				int result = g_tests.evaluateBox(i, box);
//				fprintf(stderr, "evaluate %s(%s) = %d\n", g_tests.getName(i), box.name.c_str(), result);
				if (result == 1 || result == 3 || result == 4 || result == 5 || result == 7) {
					if (result == 3)
						fprintf(stderr, "impossible identity %s(%s)\n", g_tests.getName(i), box.name.c_str());
					if (result == 4)
						fprintf(stderr, "impossible lattice %s(%s)\n", g_tests.getName(i), box.name.c_str());
					if (result == 5)
						fprintf(stderr, "impossible power %s(%s)\n", g_testCollectionFullWord.c_str(), box.name.c_str());
					if (result == 7)
						fprintf(stderr, "mom found\n");
					t.testIndex = i;
					return true;
				} else if (result == 6) {
					string w = relatorName(i);
					place.back().qr.addQuasiRelator(w);
				}
			}
		}
	}
		
	t.testIndex = -1;
	
	if (!t.lChild) {
		if (depth >= g_options.maxDepth || ++g_boxesVisited >= g_options.maxSize || ++newDepth > g_options.inventDepth) {
			string quasiRelatorDesc = place.back().qr.desc();
			fprintf(stderr, "HOLE %s (%s)\n", box.name.c_str(), quasiRelatorDesc.c_str());
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
		printf("HOLE\n");
	}
}

const char* g_programName;


static struct option longOptions[] = {
	{"box",	required_argument, NULL, 'b' },
	{"words", required_argument, NULL, 'w' },
	{"powers", required_argument, NULL, 'p'},
	{"maxDepth", required_argument, NULL, 'm' },
	{"inventDepth", required_argument, NULL, 'i' },
	{"truncateDepth", required_argument, NULL, 't' },
	{"maxSize", required_argument, NULL, 's' },
	{"ballSearchDepth", required_argument, NULL, 'B'},
	{"momsFile", required_argument, NULL, 'M'},
	{"gluingsFile", required_argument, NULL, 'G'},
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
	
	fprintf(stderr, "usage: %s %s", g_programName, optStr);
	
	for (int i = 0; longOptions[i].name; ++i) {
		fprintf(stderr, " %c : set %s\n", longOptions[i].val, longOptions[i].name);
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
		case 'm': g_options.maxDepth = atoi(optarg); break;
		case 'i': g_options.inventDepth = atoi(optarg); break;
		case 't': g_options.truncateDepth = atoi(optarg); break;
		case 's': g_options.maxSize = atoi(optarg); break;
		case 'B': g_options.ballSearchDepth = atoi(optarg); break;
		case 'M': g_options.momsFile = optarg; break;
		case 'G': g_options.gluingsFile = optarg; break;
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
	
	FindMomManifold::Environment findEnv;
	findEnv.momsFP = fopen(g_options.momsFile, "r");
	findEnv.gluingsFP = fopen(g_options.gluingsFile, "r");
	if (!findEnv.momsFP) {
		perror(g_options.momsFile);
	} else if (!findEnv.gluingsFP) {
		perror(g_options.gluingsFile);
	} else {
		FindMomManifold* findMom = FindMomManifold::create(&findEnv);
		g_tests.setMomFinder(findMom);
	}
	
	PartialTree t = readTree();
	refineTree(box, t);
	printTree(t);
	fprintf(stderr, "%d nodes added\n", g_boxesVisited);
}
