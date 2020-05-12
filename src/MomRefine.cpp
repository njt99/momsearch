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
#include <unordered_map>
#include <set>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <unistd.h>

#include "Box.h"
#include "TestCollection.h"
#include "BallSearch.h"
#include "QuasiRelators.h"

using namespace std;

struct Options {
	Options() :
    boxName(""), // Binary representation of box
    wordsFile("../words"), // Previously generated words
		powersFile("null"), // Output from power parabolic.pl
		momFile("/dev/null"), // TODO: Find what generates
		parameterizedFile("/dev/null"), // TODO: Find what generates
		maxDepth(18), // Maximum depth for a file
    truncateDepth(6), 
    inventDepth(12),
    maxSize(5000000),
		improveTree(false),
		ballSearchDepth(-1),
		fillHoles(false),
		maxWordLength(40) {}
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
TestCollection g_e2_tests;
// For each test a list of box_states
typedef vector< vector< box_state > > TestHistory;
set<string> g_momVarieties;
set<string> g_parameterizedVarieties;
static int g_boxesVisited = 0;

struct PartialTree {
	PartialTree() : lChild(NULL), rChild(NULL), testIndex(-1), testResult(open), aux_word(), qr_desc() {}
	PartialTree *lChild;
	PartialTree *rChild;
	int testIndex;
	box_state testResult;
  string aux_word;
  string qr_desc;
  string e2_killers;
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
      buf[1] = '\0';
			t.testIndex = atoi(buf);
		} else if (buf[0] == 'T') {
      buf[n-2] = '\0';
      t.qr_desc.assign(&buf[2]);
      t.testIndex = 8;
    } else {
      // Add word as eliminator to test collection
      if (strchr("mMnNgG", buf[0]) != NULL) {
        t.testIndex = g_tests.add(buf);
      } else {
        if (buf[0] == 'E') {
          char * comma = strchr(buf,',');
          comma[0] = '\0'; 
        } else {
          buf[n-2] = '\0';
        }
        t.testIndex = g_tests.add(&buf[2]);
      }
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
extern int g_max_g_len;
extern double g_maximumArea;
extern double g_minimumArea;

// Provides the word for a testIndex while
// multplied by g_xLattice copies of m and g_yLattice
// copies of n on the left. The front abelian subword is
// rewriteen with all m's first and n's second.
//string relatorName(int testIndex)
//{
//    // The global lattice counts (# of m and n's) should be set
//    // by TestCollection.evaluate(). TODO Check when these get reset
//	int xLattice = g_xLattice;
//	int yLattice = g_yLattice;
//	const char *wordName = g_tests.getName(testIndex);
//	bool done = false;
//	for (;;) {
//		switch(*wordName) {
//			case 'm': ++xLattice; break;
//			case 'M': --xLattice; break;
//			case 'n': ++yLattice; break;
//			case 'N': --yLattice; break;
//			default: done = true; break;
//		}
//		if (done) break;
//		++wordName;
//	}
//	char buf[10000];
//	char *bp = buf;
//	for (int i = 0; i < xLattice; ++i)
//		*bp++ = 'm';
//	for (int i = 0; i < -xLattice; ++i)
//		*bp++ = 'M';
//	for (int i = 0; i < yLattice; ++i)
//		*bp++ = 'n';
//	for (int i = 0; i < -yLattice; ++i)
//		*bp++ = 'N';
//	strcpy(bp, wordName);
//	return string(buf);
//}

unordered_map<int,ACJ> para_cache;
unordered_map<string,SL2ACJ> short_words_cache;

extern double g_latticeArea;
bool refineRecursive(Box box, PartialTree& t, int depth, TestHistory& history, vector< Box >& place, int newDepth, int& searchedDepth)
{
	// fprintf(stderr, "rr: %s depth %d placeSize %lu\n", box.name.c_str(), depth, place.size());
	place.push_back(box);
	int oldTestIndex = t.testIndex;
  vector<string> new_qrs;
  para_cache.clear();
  short_words_cache.clear();

  string aux_word;
	if (t.testIndex >= 0) {
    if (t.testIndex == 8) {
      stringstream qrs(t.qr_desc);
      string segment;
      while(std::getline(qrs, segment, ',')) {
        box.qr.getName(segment);
        if (g_length(segment) <= g_max_g_len) {
          t.testIndex = g_tests.add(segment);
          break;
        } 
      }            
    }

    box_state result = g_tests.evaluateBox(t.testIndex, box, aux_word, new_qrs, para_cache, short_words_cache);

    if (result != open && result != open_with_qr) {
      t.aux_word.assign(aux_word);
      t.testResult = result;
      return true;
    } else if (result == open_with_qr) {
      for (vector<string>::iterator it = new_qrs.begin(); it != new_qrs.end(); ++it) {
        box.qr.getName(*it); // Also adds qr to the box's list
      }
      t.qr_desc = box.qr.min_pow_desc();
    }
  }

	if (t.testIndex == -2 && !g_options.fillHoles) {
	  return true;
  }

  // Check if the box is now small enough that some former qrs actually kill it
  Params<ACJ> cover = box.cover();
  vector<string> quasiRelators = box.qr.words();
  for (auto it = quasiRelators.begin(); it != quasiRelators.end(); ++it) {
    // So not idenity and absUB(w.b) < 1
    SL2ACJ w = g_tests.construct_word(*it, cover, para_cache, short_words_cache); 
    if (not_identity(w)) {
      t.aux_word.assign(*it);
      t.testResult = killed_failed_qr;
      return true;
    }
  }

	if (g_options.improveTree || !t.lChild) {
		for (int i = 0; i < g_tests.size(); ++i) {
			vector<box_state>& th = history[i];
			while (th.size() <= depth && (th.size() < depth-6 || th.empty() || th.back() == open)) {
				box_state result = g_tests.evaluateCenter(i, place[th.size()]);
				th.push_back(result);
			}
			if (th.back() != open) {
        new_qrs.clear();
				box_state result = g_tests.evaluateBox(i, box, aux_word, new_qrs, para_cache, short_words_cache);

        switch (result) {
          case variety_nbd :
          case two_var_inter :
          case killed_no_parabolics :
          case killed_bad_parabolic :
          case killed_failed_qr :
          case killed_identity_impossible :
          case killed_elliptic : {
            t.aux_word.assign(aux_word);   
          }
          case killed_bounds :
          case killed_parabolics_impossible : {
            t.testIndex = i;
            t.testResult = result;
            return true;
          }
          case open_with_qr : {
            for (vector<string>::iterator it = new_qrs.begin(); it != new_qrs.end(); ++it) {
              box.qr.getName(*it); // Also adds qr to the box's list
            }
            t.qr_desc = box.qr.min_pow_desc();
            break;
          }
        }
      }
    }
    // e2 elimination
		for (int i = 9; i < g_e2_tests.size(); ++i) {
      auto it = box.e2_todo.begin();
      while(it != box.e2_todo.end()) {
        if (g_e2_tests.kills_disk_center(i, *it, box)) {
          if (g_e2_tests.kills_disk(i, *it, box, para_cache, short_words_cache)) {
            box.e2_killers.insert(i);
            it = box.e2_todo.erase(it); // returns new iterator
            continue;
          } 
        }
        ++it;
      }
      if (box.e2_todo.empty()) {
        t.testIndex = 9;
        t.testResult = killed_e2;
        for (auto it = box.e2_killers.begin(); it != box.e2_killers.end(); ++it) {
          t.e2_killers += g_e2_tests.getName(*it);
          t.e2_killers += ",";
        }
        t.e2_killers.pop_back(); // should never crash 
        // fprintf(stderr, "Killed with e2 bounds! With killers %s\n", t.e2_killers.c_str());
        return true;
      }
		}
	}

  if (g_e2_tests.size() < 6666 && (depth > 6 ||  box.name.length() > 6)) {
    set<string> e2_words = find_words(box.center(), 512, 16, box.qr.words(), true, g_e2_tests.stringIndex);
    for (auto e2_it = e2_words.begin(); e2_it != e2_words.end(); ++e2_it) {
      g_e2_tests.add(*e2_it);
    }
  }

	if (g_options.ballSearchDepth >= 0 && (g_options.improveTree || !t.lChild) && depth > 72 && depth % 12 == 0) {
		while (depth - searchedDepth > g_options.ballSearchDepth) {
			Box& searchPlace = place[++searchedDepth];
			set<string> search_words = find_words(searchPlace.center(), 6, 16, box.qr.words(), false, g_tests.stringIndex);

      for (auto s_it = search_words.begin(); s_it != search_words.end(); ++s_it) {
        string new_word = *s_it;

        int old_size = g_tests.size();
        int new_index = g_tests.add(new_word);
        history.resize(g_tests.size());

        if (old_size < g_tests.size()) {
          fprintf(stderr, "search (%s) found %s(%s)\n",
              searchPlace.qr.desc().c_str(), new_word.c_str(), searchPlace.name.c_str());

          new_qrs.clear();
          box_state result = g_tests.evaluateBox(new_index, box, aux_word, new_qrs, para_cache, short_words_cache);

          switch (result) {
            case variety_nbd : 
            case two_var_inter : 
            case killed_no_parabolics :
            case killed_bad_parabolic :
            case killed_failed_qr :
            case killed_identity_impossible :
            case killed_elliptic : {
              t.aux_word.assign(aux_word);   
            }
            case killed_bounds :
            case killed_parabolics_impossible : {
              t.testIndex = new_index;
              t.testResult = result;
              return true;
            }
            case open_with_qr : {
              for (vector<string>::iterator it = new_qrs.begin(); it != new_qrs.end(); ++it) {
                box.qr.getName(*it); // Also adds qr to the box's list
              }
              t.qr_desc = box.qr.min_pow_desc();
              break;
            }
          }
        }
      }
		}
	}

	t.testIndex = -1;

	if (!t.lChild) {
		if (depth >= g_options.maxDepth || ++g_boxesVisited >= g_options.maxSize || ++newDepth > g_options.inventDepth) {
//    fprintf(stderr,"Deph %d, max depth %d, boxes_visited %d, max size %d, newDepth %d, invent depth %d\n", depth,
//                   g_options.maxDepth, g_boxesVisited, g_options.maxSize, newDepth, g_options.inventDepth);
      Params<XComplex> params = box.center();
      Params<XComplex> nearer = box.nearer();
      double area = areaLB(nearer);
      fprintf(stderr, "HOLE %s has min area: %f center lat: %f + I %f lox: %f + I %f par: %f + I %f (%s)\n",
                      box.name.c_str(), area, params.lattice.re, params.lattice.im, params.loxodromic_sqrt.re,
                      params.loxodromic_sqrt.im, params.parabolic.re,params.parabolic.im, box.qr.desc().c_str());
      return false;
		}
		t.lChild = new PartialTree();
		t.rChild = new PartialTree();
	}

  // Subdivide the remaining e2_todo every 3 divisions of the main box 
  // TODO: might make sense to memory manage to keep only one copy of a disk
  if (depth % 6 == 0) {
    set<Disk> old_disks(box.e2_todo);
    box.e2_todo.clear();
    for (auto it = old_disks.begin(); it != old_disks.end(); ++it) {
      box.e2_todo.insert(it->child(0));
      box.e2_todo.insert(it->child(1));
    }
  }

  /* fprintf(stderr, "Number of todo boxes %d\n", box.e2_todo.size());
  if (box.e2_todo.size() < 20) {
    for (auto it = box.e2_todo.begin(); it != box.e2_todo.end(); ++it) {
      fprintf(stderr,it->desc().c_str());
    }
  } else {
    fprintf(stderr,"Box with many disks! Num g tests %d and box %s\n", g_e2_tests.size(), box.name.c_str());
  } */

	bool isComplete = true;

	isComplete = refineRecursive(box.child(0), *t.lChild, depth+1, history, place, newDepth, searchedDepth) && isComplete;
	if (place.size() > depth+1) {
		place.resize(depth+1); // truncates place for right child
  }
	for (int i = 0; i < g_tests.size(); ++i) {
		if (history[i].size() > depth) {
			history[i].resize(depth);
    }
	}
	if (searchedDepth > depth) {
		searchedDepth = depth;
  }
	if (isComplete || depth < g_options.truncateDepth) {
		isComplete = refineRecursive(box.child(1), *t.rChild, depth+1, history, place, newDepth, searchedDepth) && isComplete;
  }
	if (oldTestIndex >= 0 && t.testIndex != oldTestIndex) {
		fprintf(stderr, "invalid box %s(%s) %d %s\n", g_tests.getName(oldTestIndex), box.name.c_str(),
			treeSize(t), isComplete ? "Patched" : "Unpatched");
	}
	if (!isComplete && depth >= g_options.truncateDepth) {
		truncateTree(t);
	}
	return isComplete;
}

void refineTree(Box box, PartialTree& t)
{
	TestHistory history(g_tests.size());
	vector<Box> place;
	int searchedDepth = 0;
	refineRecursive(box, t, 0, history, place, 0, searchedDepth);
}

void printTree(PartialTree& t)
{
  char type = 'F';
  switch (t.testResult) {
    case open :
    case open_with_qr : {
      if (t.lChild && t.rChild) {
        printf("X\n");
        printTree(*t.lChild);
        printTree(*t.rChild);
      } else {
        printf("HOLE (%s)\n", t.qr_desc.c_str());
      }
      return;
    }
    case killed_bounds : {
      printf("%s\n", g_tests.getName(t.testIndex));
      return;
    }
    case killed_e2 : {
      printf("%s(%s)\n", g_e2_tests.getName(t.testIndex), t.e2_killers.c_str());
      return;
    }
    case killed_no_parabolics : type = 'K'; break;
    case variety_nbd : type = 'V'; break;
    case two_var_inter : type = 'T'; break;
    case killed_parabolics_impossible : type = 'P'; break;
    case killed_identity_impossible : type = 'I'; break;
    case killed_failed_qr : type = 'Q'; break;
    case killed_bad_parabolic : type = 'L'; break;
    case killed_elliptic : type = 'E'; break;
  }
  string killer;
  if (type == 'P') {
    killer = g_tests.getName(t.testIndex);
  } else if (type == 'E') {
    killer = g_tests.getName(t.testIndex);
    killer += "," + t.aux_word; 
  } else {
    killer = t.aux_word;
  }
  printf("%c(%s)\n", type, killer.c_str());
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
	{"minArea", required_argument, NULL, 'A'},
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
        fprintf(stderr,"Arg %c, %s\n", ch, optarg);
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
		case 'A': g_minimumArea = atof(optarg); break;
		}
	}
//    fprintf(stderr, "Max depth %d\n", g_options.maxDepth);
	
 //   usleep(30000000);

	Box box;
  int d = 0; 
	for (const char* boxP = g_options.boxName; *boxP; ++boxP) {
		if (*boxP == '0') {
			box = box.child(0);
		} else if (*boxP == '1') {
			box = box.child(1);
		}
    ++d;
    if (d % 6 == 0) {
      set<Disk> old_disks(box.e2_todo);
      box.e2_todo.clear();
      for (auto it = old_disks.begin(); it != old_disks.end(); ++it) {
        box.e2_todo.insert(it->child(0));
        box.e2_todo.insert(it->child(1));
      } 
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
