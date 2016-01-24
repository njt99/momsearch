/*
 *  TestCollection.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */
// The bounds:
// 0. |sl| >= 1 (horoball size)
// 1. Im(sl) >= 0 (only sl^2 matters)
// 2. -1/2 <= Re(n) <= 1/2 (reduction modulo M)
// 3. Im(n) >= 0 (complex conjugate symmetry)
// 4. 0 <= Im(p) <= Im(n)/2 (reduction modulo N, flipping sign of N)
// 5. 0 <= Re(p) <= 1/2 (reduction modulo M, flipping sign of M)
// 6. |sl^2| Im(n) <= 4 (area of fundamental paralleogram)
// 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "TestCollection.h"
#include "ImpossibleRelations.h"
#include <algorithm>
using namespace std;
// using namespace __gnu_cxx;

double g_maximumArea = 5.1;
int g_xLattice;
int g_yLattice;

string g_testCollectionFullWord;

int TestCollection::size()
{
	return 7 + indexString.size();
}

bool TestCollection::evaluate(string word, Params<XComplex>& params)
{
	SL2C w(1,0,0,1);
	SL2C G(constructG(params));
	SL2C g(inverse(G));
	
	string::size_type pos;
	int x = 0;
	int y = 0;
	for (pos = 0; pos <= word.length(); ++pos) {
		int c = pos < word.length() ? word[pos] : -1;
		switch(c) {
			case 'm': --x; break;
			case 'M': ++x; break;
			case 'n': --y; break;
			case 'N': ++y; break;
			default: {
				if (x != 0 || y != 0) {
					w = w*constructT(params, x, y);
					x=y=0;
				}
				if (c == 'g')
					w = w*g;
				else if (c == 'G')
					w = w*G;
			}
		}
	}
    // Rounding errors are irrelevant here, only used to guide search.
	return absLB((w.c/g.c).z) < 1;
}

SL2ACJ TestCollection::evaluate1(string word, Params<ACJ>& params)
{
	ACJ one(1), zero(0);
	SL2ACJ w(one, zero, zero, one);
	SL2ACJ G(constructG(params));
	SL2ACJ g(inverse(G));
	
	string::size_type pos;
	int x = 0;
	int y = 0;
	for (pos = 0; pos <= word.length(); ++pos) {
		int c = pos < word.length() ? word[pos] : -1;
		switch(c) {
			case 'm': --x; break;
			case 'M': ++x; break;
			case 'n': --y; break;
			case 'N': ++y; break;
			default: {
				if (x != 0 || y != 0) {
					w = w*constructT(params, x, y);
					x=y=0;
				}
				if (c == 'g')
					w = w*g;
				else if (c == 'G')
					w = w*G;
			}
		}
	}
//    XComplex a = w.a.center();
//    XComplex b = w.b.center();
//    XComplex c = w.c.center();
//    XComplex d = w.d.center();
//    fprintf(stderr, "Word: %s\n", word.c_str());
//    fprintf(stderr, "At the center is has coords\n");
//    fprintf(stderr, "a: %f + I %f\n", a.re, a.im);
//    fprintf(stderr, "b: %f + I %f\n", b.re, b.im);
//    fprintf(stderr, "c: %f + I %f\n", c.re, c.im);
//    fprintf(stderr, "d: %f + I %f\n", d.re, d.im);
//    a = G.a.center();
//    b = G.b.center();
//    c = G.c.center();
//    d = G.d.center();
//    fprintf(stderr, "Word: G\n");
//    fprintf(stderr, "At the center is has coords\n");
//    fprintf(stderr, "a: %f + I %f\n", a.re, a.im);
//    fprintf(stderr, "b: %f + I %f\n", b.re, b.im);
//    fprintf(stderr, "c: %f + I %f\n", c.re, c.im);
//    fprintf(stderr, "d: %f + I %f\n", d.re, d.im);
	return w;
}

int g_power(string w) {
    int count = 0;
    for (string::size_type p = 0; p < w.size(); ++p) {
        if (w[p] == 'g' || w[p] == 'G') ++count;
    }
    return count;
} 

bool g_power_sort(string a, string b) { return g_power(a) < g_power(b); }

/* Tests is the box is within the variety neighborhood for 
   its shortest g power quasi-relators. This test is used to 
   stop subdivision of a HOLE. The relevant Lemma:

{\bf Lemma (Variety Buffer-Zone Lemma):} {\it Let $W(p) =  (a\ \ b\ \ c\ \ d)$ be the matrix in ${\bf PSL}(2,{\bf C})$ determined by the variety word $W = W(m,n,g)$ at the point $p \in {\cal P}$, let $V_W$ be the variety determined by $W = I$ in ${\cal P}$, and let $N_W$ be the neighborhood of $V_W$  determined by the conditions that the absolute value of the $c$ co-ordinate of $W(p)$ is less than $1$ and (if the absolute value of the $c$ co-ordinate of $W(p)$ is zero) the absolute value of the $b$ coordinate is less than $1$.  Then no point in $N_W - V_W$ corresponds to a discrete, torsion-free hyperbolic 3-manifold.}

*/

bool TestCollection::box_inside_nbd(NamedBox& box)
{
	Params<ACJ> params = box.cover();
    vector<string> qrs(box.qr.wordClasses());
    if (qrs.empty()) { return false; }
    sort(qrs.begin(), qrs.end(), g_power_sort);
    int min_power = g_power(qrs.front());
	for (vector<string>::iterator it = qrs.begin(); it != qrs.end(); ++it) {
        if (g_power(*it) > min_power) {
            return true;
        } else {
		    SL2ACJ w(evaluate1(*it, params));
            if (!(absUB(w.c) < 1 && absUB(w.b) < 1)) { return false; } 
        }
    }
    return true; 
}

bool TestCollection::validIdentity(string word, Box& box)
{
    // Checks to see is ALL  cyclic permutations of a word is identity somewhere in the box
	Params<ACJ> params = box.cover();
	for (string::size_type pos = 0; pos < word.size(); ++pos) {
		string pword = word.substr(pos, word.size()-pos) + word.substr(0, pos);
		SL2ACJ w(evaluate1(pword, params));
		if (notIdentity(w)) {
			return false;
//        } else {
//            XComplex a = w.a.center();
//            XComplex b = w.b.center();
//            XComplex c = w.c.center();
//            XComplex d = w.d.center();
//            fprintf(stderr, "This word is identity somewhere in the box\n");
//            fprintf(stderr, "Word: %s\n", word.c_str());
//            fprintf(stderr, "At the center is has coords\n");
//            fprintf(stderr, "a: %f + I %f\n", a.re, a.im);
//            fprintf(stderr, "b: %f + I %f\n", b.re, b.im);
//            fprintf(stderr, "c: %f + I %f\n", c.re, c.im);
//            fprintf(stderr, "d: %f + I %f\n", d.re, d.im);
        }
	}
	return true;
}

int TestCollection::evaluate(string word, Params<ACJ>& params, bool isNotParabolic)
{
	SL2ACJ w(evaluate1(word, params));
	SL2ACJ G(constructG(params));
	SL2ACJ g(inverse(G));
	if (isNotParabolic) {
		return absLB(w.c) > 0
		|| (absLB(w.a-1) > 0 && absLB(w.a+1) > 0)
		|| (absLB(w.d-1) > 0 && absLB(w.d+1) > 0);
	}
	ACJ r(w.c / g.c);
	if (absUB(r) < 1) {
		if (absLB(w.c) > 0
		 || (absLB(w.a-1) > 0 && absLB(w.a+1) > 0)
		 || (absLB(w.d-1) > 0 && absLB(w.d+1) > 0)) {
			return 1;
		} else {
			list<string> mandatory;
			bool isImpossible = impossible->isAlwaysImpossible(word, mandatory);
			if (isImpossible)
				return 3;
			// TODO: ULP and verify this argument
			XComplex t = (w.b.f / w.a.f).z;
			int y = int(floor(t.im / params.lattice.f.im));
			int numFound = 0;
			int xLattice, yLattice;
			for (int yi = -1; yi <= 2; ++yi) {
				XComplex tY = (t - (params.lattice.f * XComplex(y+yi)).z).z;
				int x = int(floor(tY.re));
				for (int xi = -1; xi <= 2; ++xi) {
					ACJ tL = w.b - w.a*(ACJ(y+yi) * params.lattice + ACJ(x+xi));
					if (absLB(tL) == 0) {
						++numFound;
						if (numFound > 1)
							break;
						xLattice = x + xi;
						yLattice = y + yi;
					}
				}
			}
			
			if (numFound == 0) { // cannot be on lattice
				return 4;
			}
			if (numFound == 1) { // exactly one lattice point possible
				// TODO: prove this, and possibly get better bounds on the number
				// of points which need to be checked
				isImpossible = impossible->isImpossible(word, xLattice, yLattice, mandatory);
				if (isImpossible) {
					return 3;
				} else {
					g_xLattice = xLattice;
					g_yLattice = yLattice;
					return 6;
				}
			}
			for (list<string>::iterator it = mandatory.begin(); it != mandatory.end(); ++it) {
				if (evaluate(*it, params, true))
					return 5;
			}
			return 2;
		}
	} else {
		return 0;
	}
}

double g_latticeArea;
int TestCollection::evaluateCenter(int index, Box& box)
{
	Params<XComplex> params = box.center();
	switch(index) {
		case 0:	{
			XComplex sl = params.loxodromicSqrt;
			return sl.re*sl.re + sl.im*sl.im < 1.0;
		}
		case 1: return params.loxodromicSqrt.im < 0.0
		 || params.lattice.im < 0.0
		 || params.parabolic.im < 0.0
		 || params.parabolic.re < 0.0;
		case 2: return fabs(params.lattice.re) > 0.5;
		case 3: return absUB(params.lattice) < 1;
		case 4: return params.parabolic.im > 0.5*params.lattice.im;
		case 5: return params.parabolic.re > 0.5;
		case 6: {
			g_latticeArea = pow(absLB(params.loxodromicSqrt),2)*params.lattice.im;
			return g_latticeArea > g_maximumArea;
		}
		default:
			return evaluate(indexString[index-7], params);
	}
}

int TestCollection::evaluateBox(int index, NamedBox& box)
{
	Params<XComplex> nearest = box.nearest();
	Params<XComplex> furthest = box.furthest();
	Params<XComplex> maximum = box.maximum();
//	enumerate("");
	switch(index) {
		case 0: {
			XComplex maxSl = furthest.loxodromicSqrt;
			return maxSl.re*maxSl.re + maxSl.im*maxSl.im < 1.0;
		}
		case 1: return maximum.loxodromicSqrt.im < 0.0
         || maximum.lattice.im < 0.0
		 || maximum.parabolic.im < 0.0
		 || maximum.parabolic.re < 0.0;
		case 2: return fabs(nearest.lattice.re) > 0.5;
		case 3: return absUB(furthest.lattice) < 1;
        // Note: we can exclude the box if and only if the parabolic imag part is
        // bigger than half the lattice imag part over the WHOLE box
        // We assume that case 1 has been tested. Multiplication by 0.5 is EXACT (if no underflow or overflow)
        // TODO: ULP
		case 4: return nearest.parabolic.im > 0.5*furthest.lattice.im;
		case 5: return nearest.parabolic.re > 0.5;
		case 6: {
			Params<ACJ> cover(box.cover());
			double absLS = absLB(cover.loxodromicSqrt);
			double area = dec_d(dec_d(absLS * absLS) * nearest.lattice.im);
			if (area > g_maximumArea) {
				return true;
			} else {
				return false;
			}
		}
		default: {
			Params<ACJ> cover(box.cover());
			int result = evaluate(indexString[index-7], cover, false);
//			if (result) {
//                // TODO: Understand this tail enumeration that adds words based on given word
//				enumerate(indexString[index-7].c_str());
//			}
			return result;
		}
	}
}

// Returns the index number for the first basic 7 tests
// or the quasi-relator if the index is 7 or above
const char* TestCollection::getName(int index)
{
	static char buf[4];
	if (index < 7) {
		sprintf(buf, "%d", index);
		return buf;
	} else {
		return indexString[index-7].c_str();
	}
}


int TestCollection::add(string word)
{
	map<string, int>::iterator it = stringIndex.find(word);
	if (it == stringIndex.end()) {
//		fprintf(stderr, "adding %lu=%s\n", indexString.size(), word.c_str());
		stringIndex[word] = indexString.size();
		indexString.push_back(word);
		return indexString.size()+6;
	}
	else
		return it->second+7;
}

void TestCollection::load(const char* fileName)
{
	FILE *fp = fopen(fileName, "r");
	char buf[1024];
	while (fp && fgets(buf, sizeof(buf), fp)) {
		int n = strlen(buf);
		if (!isalpha(buf[n-1]))
			--n;
		add(string(buf, n));
	}
}

void TestCollection::loadImpossibleRelations(const char* fileName)
{
	impossible = ImpossibleRelations::create(fileName);
}

//int g_maxWordLength = 2;
//void TestCollection::enumerate(const char* w)
//{
//	static vector<int> maxP;
//	if (!*w && !maxP.empty())
//		return;
//	int pCount=0, lCount=0;
//	while (*w) {
//		if (*w == 'g' || *w == 'G')
//			++lCount;
//		else
//			++pCount;
//		++w;
//	}
//	if (lCount == 0) lCount = 1;
//	if (lCount >= maxP.size()) {
//		maxP.resize(lCount+1, -1);
//	}
//	if (pCount <= maxP[lCount]) {
//		return;
//	}
//	
//	if (lCount + pCount > g_maxWordLength)
//		g_maxWordLength = lCount + pCount;
//	
//	maxP[lCount] = pCount;
//	
//	if (pCount > 2)
//		pCount = 2;
//	if (lCount > 2)
//		lCount = 2;
////	printf("ENUMERATING %d,%d\n", pCount, lCount);
////	enumerateTails("", pCount, lCount);
//}
//
//void TestCollection::enumerateTails(string s, int pCount, int lCount)
//{
//	if (pCount < -1 || lCount < -1 || (pCount == -1 && lCount == -1))
//		return;
//	const char* p = "Gg";
//	if (s.size() > 0) {
//		char last = s[s.size()-1];
//		switch(last) {
//			case 'G': p = "GMmNn"; break;
//			case 'g': p = "gMmNn"; break;
//			case 'M': p = "GgMNn"; break;
//			case 'm': p = "GgmNn"; break;
//			case 'N': p = "GgN"; break;
//			case 'n': p = "Ggn"; break;
//		}
//	}
//	for (; *p; ++p) {
//		string n = s;
//		n.append(1, *p);
//		if (*p == 'g' || *p == 'G') {
//			add(n);
//			enumerateTails(n, pCount, lCount-1);
//		} else {
//			enumerateTails(n, pCount-1, lCount);
//		}
//	}
//}
//
//string checkPower(string word, int x, int y)
//{
//	char buf[200];
//	char *bp = buf;
//	if (abs(x) > 10 || abs(y) > 10)
//		return "";
//	while (x > 0) { *bp++ = 'm'; --x; }
//	while (x < 0) { *bp++ = 'M'; ++x; }
//	while (y > 0) { *bp++ = 'n'; --y; }
//	while (y < 0) { *bp++ = 'N'; ++y; }
//	strcpy(bp, word.c_str());
//	int l = strlen(buf);
//	for (int n = 2; n+n <= l; ++n) {
//		int k;
//		for ( k = 1; k*n < l; ++k) ;
//		if (k*n == l) {
//			for (--k; k > 0; --k) {
//				if (strncmp(buf, buf+k*n, n))
//					break;
//			}
//			if (k == 0) {
////				fprintf(stderr, "id %s x%d\n", buf, n);
//				g_testCollectionFullWord = buf;
//				buf[n] = '\0';
//				return buf;
//			}
//		}
//	}
////	fprintf(stderr, "fullWord = %s\n", buf);
//	return "";
//}
