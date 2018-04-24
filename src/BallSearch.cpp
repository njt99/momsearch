/*
 *  BallSearch.cpp
 *  horoballs
 *
 *  Created by Nathaniel Thurston on 23/04/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "BallSearch.h"
#include "CanonicalName.h"
#include "SL2C.h"
#include "Params.h"
#include <list>
#include <map>
#include <queue>
#include <algorithm>
using namespace std;

namespace BallSearchImpl {
	struct Word {
		string name;
		string nameClass; // cuts off leading or tailing M,N,m,n products
		SL2C matrix;
	};
	
	struct LatticePoint {
		Params<XComplex> params;
		int x;
		int y;
		double distance;
	};
	
	struct LowerDistance : public binary_function<const LatticePoint&, const LatticePoint&, bool>
	{
		bool operator() (const LatticePoint& a, const LatticePoint& b)
		{
			return a.distance < b.distance;
		}
	};

	int gPower(Word w)
	{
		int gCount = 0;
		for (string::size_type pos = 0; pos < w.name.size(); ++pos)
			if (w.name[pos] == 'g' || w.name[pos] == 'G')
				++gCount;
		return gCount;
	}

	
	Word operator * (const Word& lhs, const LatticePoint& rhs); // Multiply word by lattice transtions
	Word operator * (const Word& lhs, const Word& rhs); // Multiply/concatiante two words
	
	struct WordPair {
		WordPair(Word& first, Word& second, Params<XComplex>& params);
		void setCurrentIndex(int i);
		
		Word firstWord;
		Word secondWord;
		vector<LatticePoint> distances;
		int distanceIndex;
		Word currentCombination;
		double currentBallSize;
	};
	
	struct SmallerBallSize : public binary_function<const WordPair&, const WordPair&, bool>
	{
		bool operator() (const WordPair& a, const WordPair& b)
		{
			return a.currentBallSize < b.currentBallSize;
		}
	};
	
	struct BallSearch {
		BallSearch(Params<XComplex> params_) :params(params_), G(constructG(params)), g(inverse(G)), m_foundBigBall(false) {}
		~BallSearch() {}
		void pushWord(Word word);
		void pushWord(string word);
		Word findWord();
		bool foundBigBall() { return m_foundBigBall; }
		void findNames(Word& word);
		void addRelator(string word);
		
		Params<XComplex> params;
		CanonicalName canonicalName;
		SL2C G;
		SL2C g;
		vector<Word> words;
		priority_queue<WordPair, vector<WordPair>, SmallerBallSize> pairs;
		bool m_foundBigBall;
	};
	
	void BallSearch::findNames(Word& word)
	{
		word.name = canonicalName.getCanonicalName(word.name);
		string::size_type firstG = word.name.find_first_of("gG");
		string::size_type lastG = word.name.find_last_of("gG");
		if (firstG == string::npos) {
			word.nameClass = "";
		} else {
			word.nameClass = word.name.substr(firstG, lastG - firstG + 1);
		}
	}
	
	Word inverse(Word& word)
	{
		Word inv;
		inv.name = word.name;
		reverse(inv.name.begin(), inv.name.end());
		string::size_type pos;
		for (pos = 0; pos < inv.name.size(); ++pos) {
			char c = inv.name[pos];
			if (c >= 'a' && c <= 'z')
				inv.name[pos] += 'A' - 'a';
			else
				inv.name[pos] -= 'A' - 'a';
		}
		inv.matrix = inverse(word.matrix);
		return inv;
	}
	
	Word operator * (const Word& lhs, const LatticePoint& rhs)
	{
		Word result;
		result.name = lhs.name;
		result.nameClass = lhs.nameClass;
		for (int i = 0; i <  rhs.x; ++i) result.name += "M";
		for (int i = 0; i < -rhs.x; ++i) result.name += "m";
		for (int i = 0; i <  rhs.y; ++i) result.name += "N";
		for (int i = 0; i < -rhs.y; ++i) result.name += "n";
		result.matrix = lhs.matrix * constructT(rhs.params, rhs.x, rhs.y);
		return result;
	}
	
	Word operator * (const Word& lhs, const Word& rhs)
	{
		Word result;
		result.name = lhs.name + rhs.name;
		result.matrix = lhs.matrix * rhs.matrix;
		return result;
	}

	WordPair::WordPair(Word& first, Word& second, Params<XComplex>& params)
		:firstWord(first), secondWord(second), distanceIndex(0)
	{
//        fprintf(stderr, "Buidling word pair\n");
//        fprintf(stderr, "a_1 = %f + %f i, c_1 = %f + %f i\n a_2 =  %f + %f i, c_2 = %f + %f i\n",
//                        firstWord.matrix.a.re, firstWord.matrix.a.im, firstWord.matrix.c.re, firstWord.matrix.c.im,
//                        secondWord.matrix.a.re, secondWord.matrix.a.im, secondWord.matrix.c.re, secondWord.matrix.c.im); 
		XComplex centerDiff = (firstWord.matrix.a / firstWord.matrix.c
		 - secondWord.matrix.a / secondWord.matrix.c).z;
        if (absUB(centerDiff) < infinity()) {
            int y0 = int(floor(centerDiff.im / params.lattice.im));
            int x0 = int(floor(centerDiff.re - y0*params.lattice.re));
//            fprintf(stderr, "%d, %d\n", x0, y0); 
            for (int x = -2-x0; x <= 3-x0; ++x) {
                for (int y = -1-y0; y <= 2-y0; ++y) {
                    // this is here to avoid having to reduce first^(-1) * second
                    if (x == 0 && y == 0 && firstWord.name[0] == secondWord.name[0])
                        continue;
                    LatticePoint l;
                    l.params = params;
                    l.x = -x;
                    l.y = -y;
                    XComplex t = (centerDiff + double(x) + double(y)*params.lattice).z;
                    l.distance = pow(absUB(t),2);
//                    fprintf(stderr, "%d, %d\n", -x, -y); 
                    if (abs(l.x) < 4 && abs(l.y) < 4) 
                        distances.push_back(l);
                }
            }
        }
		sort(distances.begin(), distances.end(), LowerDistance());
		setCurrentIndex(0);
	}
	
	void WordPair::setCurrentIndex(int i)
	{
		distanceIndex = i;
		if (i >= distances.size()) {
			currentBallSize = 0;
		} else {
			currentCombination = (inverse(firstWord) * distances[i]) * secondWord;
            // Looks like this is an heuristic ball size 
			currentBallSize = 0.5 / pow(absLB(currentCombination.matrix.c),2);
		}
//		printf("WordPair(%s,%s):setCurrentIndex(%d) : %d,%d %f %s size=%f\n",
//			firstWord->name.c_str(), secondWord->name.c_str(),
//			i, distances[i].x, distances[i].y, distances[i].distance,
//			currentCombination.name.c_str(), currentBallSize);
	}
	
	void BallSearch::pushWord(Word word)
	{
		vector<Word>::iterator it;
		// first, check to see if this word has already been pushed
		findNames(word);
		if (word.nameClass == "") {
			return;
        }
		for (it = words.begin(); it != words.end(); ++it) {
			if (it->nameClass == word.nameClass) {
				return;
            }
        }
//		fprintf(stderr,"pushWord(%s) class=%s\n", word.name.c_str(), word.nameClass.c_str());
		words.push_back(word);
		Word* wp = &words.back();
		for (it = words.begin(); it != words.end(); ++it) {
//            fprintf(stderr,"Trying to print\n");
//            fprintf(stderr, "First word %s, Second word %s\n", it->name.c_str(), word.name.c_str());
			WordPair pair(*it, word, params);
//			fprintf(stderr,"pushWordPair(%s,%s) distance size %d\n", pair.firstWord.name.c_str(), pair.secondWord.name.c_str(), pair.distances.size());
			if (pair.distances.size() > 0) {
				pairs.push(pair);
            }
		}
//        fprintf(stderr,"Done with pushWord\n");
	}
	
	void BallSearch::pushWord(string word)
	{
		Word w;
		w.name = word;
		int x = 0;
		int y = 0;
        string::reverse_iterator rit;
        for (rit = word.rbegin(); rit != word.rend(); ++rit) {
            char h = *rit;
            switch(h) {
				case 'm': --x; break;
				case 'M': ++x; break;
				case 'n': --y; break;
				case 'N': ++y; break;
				default: {
					if (x != 0 || y != 0) {
						w.matrix = constructT(params, x, y) * w.matrix;
						x = y = 0;
					}
					if (h == 'g') {
						w.matrix = g * w.matrix;
					} else if (h == 'G') {
						w.matrix = G * w.matrix;
					}
				}
			}
		}
        if (x != 0 || y != 0) {
            w.matrix = constructT(params, x, y) * w.matrix;
        }
//        XComplex a = w.matrix.a;
//        XComplex b = w.matrix.b;
//        XComplex c = w.matrix.c;
//        XComplex d = w.matrix.d;
//        fprintf(stderr, "Word: %s\n", w.name.c_str());
//        fprintf(stderr, "At the center is has coords\n");
//        fprintf(stderr, "a: %f + I %f\n", a.re, a.im);
//        fprintf(stderr, "b: %f + I %f\n", b.re, b.im);
//        fprintf(stderr, "c: %f + I %f\n", c.re, c.im);
//        fprintf(stderr, "d: %f + I %f\n", d.re, d.im);
		//printf("pushWord(%s): distance=%f\n", w.name.c_str(), 0.5 / norm(w.matrix.c));
		pushWord(w);
		Word wInv = inverse(w);
		findNames(wInv);
		pushWord(wInv);
		//fprintf(stderr, "pushed words %s(%s) and %s(%s)\n",
			//w.name.c_str(), w.nameClass.c_str(),
			//wInv.name.c_str(), wInv.nameClass.c_str());
	}
	
	Word BallSearch::findWord()
	{
		while (pairs.size() > 0) {
			WordPair largest(pairs.top());
			Word result(largest.currentCombination);
			string oldName = result.name;
			findNames(result);			
			pairs.pop();
			largest.setCurrentIndex(largest.distanceIndex+1);
			pairs.push(largest);
			vector<Word>::iterator it;
			for (it = words.begin(); it != words.end(); ++it)
				if (result.nameClass == it->nameClass)
					break;
			if (result.nameClass != "" && it == words.end()) {
				double sizeRatio = absLB(result.matrix.c) / absUB(words.front().matrix.c);
//				fprintf(stderr, "%s: sizeRatio = %f gPower = %d\n", result.name.c_str(), sizeRatio, gPower(result));
				if (sizeRatio < 1) {
					m_foundBigBall = true;
				}
				return result;
			}
			//fprintf(stderr, "rejecting %s^-1 *(%d,%d) %s = %s -> %s (%s)\n",
				//largest.firstWord->name.c_str(),
				//largest.distances[largest.distanceIndex-1].x,
				//largest.distances[largest.distanceIndex-1].y,
				//largest.secondWord->name.c_str(),
				//oldName.c_str(),
				//result.name.c_str(),
				//result.nameClass.c_str()
			//);
			//printf("duplicate; resorting\n");
		}
        return Word();
	}
	
	void BallSearch::addRelator(string w)
	{
		canonicalName.addRelator(w);
	}
}

vector<string> findWords(Params<XComplex> center, vector<string> seedWords, int numWords, int maxLength,
	vector<string> relators)
{
//    fprintf(stderr, "Params:\n");
//    fprintf(stderr, "L: %f + I %f \n", center.lattice.re, center.lattice.im);
//    fprintf(stderr, "S: %f + I %f \n", center.loxodromic_sqrt.re, center.loxodromic_sqrt.im);
//    fprintf(stderr, "P: %f + I %f \n", center.parabolic.re, center.parabolic.im);
	BallSearchImpl::BallSearch search(center);
//    fprintf(stderr, "Params:\n");
//    fprintf(stderr, "L: %f + I %f \n", search.params.lattice.re, search.params.lattice.im);
//    fprintf(stderr, "S: %f + I %f \n", search.params.loxodromic_sqrt.re, search.params.loxodromic_sqrt.im);
//    fprintf(stderr, "P: %f + I %f \n", search.params.parabolic.re, search.params.parabolic.im);
	for (int i = 0; i < relators.size(); ++i)
		if (relators[i].size() > 0)
			search.addRelator(relators[i]);
	for (int i = 0; i < seedWords.size(); ++i)
		if (seedWords[i].size() > 0)
			search.pushWord(seedWords[i]);
	search.pushWord("g");
	search.pushWord("G");
	vector<string> foundWords;
	while (numWords > int(foundWords.size()) || (-numWords > int(foundWords.size()) && !search.foundBigBall())) {
//        fprintf(stderr, "While loop\n");
		BallSearchImpl::Word w = search.findWord();
        int g_power = gPower(w);
		if (g_power == 0 || g_power > maxLength) {
			search.m_foundBigBall = false;
			continue;
		}
		//fprintf(stderr, "adding %s\n", w.name.c_str());
		search.pushWord(w);
		foundWords.push_back(w.nameClass);
	}
	return foundWords;
}
