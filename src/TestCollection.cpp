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

double g_maximumArea = 5.24;
int g_xLattice;
int g_yLattice;

string g_testCollectionFullWord;

int TestCollection::size()
{
	return 7 + indexString.size();
}

SL2C TestCollection::construct_word(string word, Params<XComplex>& params)
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
	return w;
}

SL2ACJ TestCollection::construct_word(string word, Params<ACJ>& params)
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

/* Tests if the box is within the variety neighborhood for 
   its shortest g power quasi-relators. This test is used to 
   stop subdivision of a HOLE. The relevant Lemma:

{\bf Lemma (Variety Buffer-Zone Lemma):} {\it Let $W(p) =  (a\ \ b\ \ c\ \ d)$ be the matrix in ${\bf PSL}(2,{\bf C})$ determined by the variety word $W = W(m,n,g)$ at the point $p \in {\cal P}$, let $V_W$ be the variety determined by $W = I$ in ${\cal P}$, and let $N_W$ be the neighborhood of $V_W$  determined by the conditions that the absolute value of the $c$ co-ordinate of $W(p)$ is less than $1$ and (if the absolute value of the $c$ co-ordinate of $W(p)$ is zero) the absolute value of the $b$ coordinate is less than $1$.  Then no point in $N_W - V_W$ corresponds to a discrete, torsion-free hyperbolic 3-manifold.}

*/

bool TestCollection::box_inside_nbd(NamedBox& box, string& var_word)
{
    // Checks if a minimal g-power quasi relator pusts the box in a neighborhood.
    // Returns true if found and copies word into var_word
    Params<ACJ> params = box.cover();
    vector<string> qrs = box.qr.wordClasses();
    if (qrs.empty()) { return false; }
    sort(qrs.begin(), qrs.end(), g_power_sort);
    int min_power = g_power(qrs.front());
	for (vector<string>::iterator it = qrs.begin(); it != qrs.end(); ++it) {
        if (g_power(*it) > min_power) {
            break;
        } else {
            SL2ACJ w = construct_word(*it, params);
            if (inside_var_nbd(w)) {
                var_word = *it;
                return true;
            } 
        }
    }
    return false;
}

bool TestCollection::box_inside_at_least_two_nbd(NamedBox& box, vector<string>& var_words)
{
    // Check if there are two quasi-relators that put the box in a neighborhood. 
    // Returns true of two were found and places them in var_words.
    Params<ACJ> params = box.cover();
    vector<string> qrs = box.qr.wordClasses();
    if (qrs.size() < 2) { return false; }
    sort(qrs.begin(), qrs.end(), g_power_sort);
    int min_power = g_power(qrs.front());
    vector<string> found;
	for (vector<string>::iterator it = qrs.begin(); it != qrs.end(); ++it) {
        if (g_power(*it) > min_power && found.size() > 1) {
            break;
        } else {
            SL2ACJ w = construct_word(*it, params);
            if (inside_var_nbd(w)) {
                found.push_back(*it);
            } 
        }
    }
    if (found.size() > 1) {
        var_words.swap(found);
        return true;
    }
    return false; 
}


bool TestCollection::valid_identity_cyclic(string word, Params<ACJ>& params)
{
    // Checks to see is ALL  cyclic permutations of a word is identity somewhere in the box
    if (!word.size()) return false;
	for (string::size_type pos = 0; pos < word.size(); ++pos) {
		string pword = word.substr(pos, word.size()-pos) + word.substr(0, pos);
		SL2ACJ w = construct_word(pword, params);
		if (not_identity(w)) {
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

bool TestCollection::valid_variety(string word, Params<ACJ>& params)
{
    // Checks to see is ALL  cyclic box is small enough for all cyclic permutations to be inside variety neighborhood
    if (!word.size()) return false;
	for (string::size_type pos = 0; pos < word.size(); ++pos) {
            string pword = word.substr(pos, word.size()-pos) + word.substr(0, pos);
            SL2ACJ w = construct_word(pword, params);
            if (!(absUB(w.c) < 1 && absUB(w.b) < 1)) { return false; }
	}
	return true;
}

bool TestCollection::valid_intersection(NamedBox& box) {
    // TODO: Check this does what is should. Should show that two varieties intersect in box
	Params<ACJ> params = box.cover();
    vector<string> qrs(box.qr.wordClasses());
    if (qrs.size() < 2) { return false; }
    sort(qrs.begin(), qrs.end(), g_power_sort);
    int min_power = g_power(qrs.front());
    int found = 0;
    vector<string> var_jets;
	for (vector<string>::iterator it = qrs.begin(); it != qrs.end(); ++it) {
        if (g_power(*it) > min_power && found > 1) {
            break;
        } else {
            if (!valid_variety(*it, params)) { return false; }
            var_jets.push_back(*it);
            found += 1;
        }
    }
    if (found > 1) {
        string w0 = var_jets[0];
        string w1 = var_jets[1];
        for (string::size_type idx_0 = 0; idx_0 < w0.size(); ++idx_0) {
            string w0_p = w0.substr(idx_0, w0.size()-idx_0) + w0.substr(0, idx_0);
            SL2ACJ w0_j(construct_word(w0_p, params));
            for (string::size_type idx_1 = 0; idx_1 < w1.size(); ++idx_1) {
                string w1_p = w1.substr(idx_1, w1.size()-idx_1) + w1.substr(0, idx_1);
                SL2ACJ w1_j(construct_word(w1_p, params));
                if (notZero(w1_j - w0_j) && notZero(w1_j + w0_j)) { return false; }
            }
        }
        return true;
    } else {
        return false;
    }
}

box_state TestCollection::evaluate_approx(string word, Params<XComplex>& params)
{
    SL2C w = construct_word(word, params);
    if (maybe_variety(w) ) return variety_center;
    if (!maybe_large_horoball(w,params)) return open;
    return large_horoball_center;
}

bool TestCollection::ready_for_parabolics_test(SL2ACJ& w)
{
     double one = 1; // Exact
    // We check the box is small enough to determine the sign of translation
    return ( absUB(w.d - one) < 2 || absUB(w.d + one) < 2 || 
             absUB(w.a - one) < 2 || absUB(w.a + one) < 2 );
}

bool TestCollection::only_bad_parabolics(SL2ACJ& w, Params<ACJ>& params)
{
    // Tests if w hits any lattice points (when w is parabolic).
    // This test is inconclusive is w has large transtalion (i.e. translate
    // w's center into the first postive quad of the lattice.)

    // WE ASSUME ready_for_parabolics_test(w) == true

    // For all parabolic points in the box, we want verify
    // that none of them are lattice points. At such a point w.a = +/- 1, so
    //      translation = +/- w.b
    // Away from the parabolic locus, everything is assumed elimanited.

    // There are now 4 equations to check corresponding to the intersection
    // of 4 cirles :
    // |translation - 0          | < |1 + lattice|
    // |translation - (1+lattice)| < |1 + lattice|
    // |translation - 1          | < |1 - lattice|
    // |translation - lattice    | < |1 - lattice|
    // These inequailties show that transltion is not on the lattice (assuming
    // parameterd space constraitns). See proof in text.
    // 
    // To make the computation efficient, rearange and take absolute values at the end.
    //

    double one = 1; // Exact
    ACJ T = (absUB(w.d - one) < 2 || absUB(w.a - one) < 2) ? w.b : -w.b;
    ACJ L = params.lattice;

    ACJ d1 = T / (L + one);
    ACJ d2 = d1 - one; // uses fewer operations
    ACJ d3 = (T - one) / (L - one);
    ACJ d4 = d3 - one; // better error estimate

    return (absUB(d1) < 1 && absUB(d2) < 1 && absUB(d3) < 1 && absUB(d4) < 1);
}

string shifted_word(const string& word, int M_pow, int N_pow) {
    string shifted(word);
    string::size_type first_g = shifted.find_first_of("gG");
    if (first_g > 0) {
        for (int j = 0; j < first_g; ++j) {
            switch (shifted[j]) {
                case 'm' : --M_pow; break; 
                case 'M' : ++M_pow; break;
                case 'n' : --N_pow; break;
                case 'N' : ++N_pow; break;
            }
        } 
    }
    shifted = shifted.substr(first_g, string::npos);
    for (int i = 0; i <  N_pow; ++i) shifted.insert(0,"N");
    for (int i = 0; i < -N_pow; ++i) shifted.insert(0,"n");
    for (int i = 0; i <  M_pow; ++i) shifted.insert(0,"M");
    for (int i = 0; i < -M_pow; ++i) shifted.insert(0,"m");
     
    return shifted;
}

box_state TestCollection::evaluate_ACJ(string word, Params<ACJ>& params, string& aux_word)
{
    box_state state = open;
    aux_word = word;
	SL2ACJ w = construct_word(word, params);

    if (inside_var_nbd(w)) return variety_nbd;

	if (large_horoball(w,params)) {

        if (not_para_fix_inf(w)) {

			return killed_no_parabolics;

		} else {

			list<string> mandatory;
			bool isImpossible = impossible->isAlwaysImpossible(word, mandatory);

			if (isImpossible) return killed_parabolics_impossible;
   
            // Look for lattice points. We guess at the center
            if (ready_for_parabolics_test(w)) { // Makes no sese to do this otherwise
                ACJ L = params.lattice;
                XComplex cL = L.f;
                XComplex cT = w.b.f;
                // We expect T to be near the lattice point M_pow + N_pow*L
                double N_pow = floor(cT.im / cL.im);
                double M_pow = floor((cT - (cL * N_pow).z).z.re);
                // We look over 16 nearby lattice points
                for (int y_i = -1; y_i <= 2; ++y_i) {
                    double N_i = N_pow + y_i;
                    for (int x_i = -1; x_i <= 2; ++x_i) {
                        double M_i = M_pow + x_i;
                        ACJ L_i = L * N_i + M_i; 
                        SL2ACJ w_i(w.a, w.b - L_i, w.c, w.d); // Cheaper than constucting new word
                        // What if we now have a variety word?
                        if (inside_var_nbd(w_i)) { // TODO: Test with constucted word!
                            state = variety_nbd;
                        }
                        else if (only_bad_parabolics(w_i, params)) { // TODO: Test with constucted word!
                            // w_i is a bad parabolic
                            state = killed_bad_parabolic;
                        } else if (absUB(w_i.b) < 1) {
                            // w_i is a quai-relator
                            isImpossible = impossible->isImpossible(word, M_i, N_i, mandatory);
                            if (isImpossible) {
                                state = killed_identity_impossible;
                            }
                            // Mandaotry includes list of things that must be parabolic. If they are not parabolic
                            // anywhere in the box, we can kill the box
                            for (list<string>::iterator it = mandatory.begin(); it != mandatory.end(); ++it) {
	                            SL2ACJ w_sub = construct_word(*it, params);
                                if (not_para_fix_inf(w_sub)) {
                                    aux_word = *it;
                                    return killed_elliptic;
                                }
                            }

                            // If nothing has worked, at least add w as a quasi relator
                            state =  open_with_qr;
                        }
                        if (state != open) {
                            aux_word = shifted_word(word, - int(M_i), - int(N_i));
                            return state;
                        }
                    }
                }
			}
        }
    }            
    return state;
}

box_state check_bounds_center(bool result) {
    if (result) return out_of_bounds_center;
    else return open;
}

double g_latticeArea;
box_state TestCollection::evaluateCenter(int index, Box& box)
{
	Params<XComplex> params = box.center();
	switch(index) {
		case 0:	{
			XComplex sl = params.loxodromic_sqrt;
			return check_bounds_center(sl.re*sl.re + sl.im*sl.im < 1.0);
		}
		case 1: return check_bounds_center( params.loxodromic_sqrt.im < 0.0
                                         || params.lattice.im < 0.0
                                         || params.parabolic.im < 0.0
                                         || params.parabolic.re < 0.0);
		case 2: return check_bounds_center(fabs(params.lattice.re) > 0.5);
		case 3: return check_bounds_center(absUB(params.lattice) < 1);
		case 4: return check_bounds_center(params.parabolic.im > 0.5*params.lattice.im);
		case 5: return check_bounds_center(params.parabolic.re > 0.5);
		case 6: {
			g_latticeArea = pow(absLB(params.loxodromic_sqrt),2)*params.lattice.im;
			return check_bounds_center(g_latticeArea > g_maximumArea);
		}
		default:
			return evaluate_approx(indexString[index-7], params);
	}
}

box_state check_bounds(bool result) {
    if (result) return killed_bounds;
    else return open;
}

box_state TestCollection::evaluateBox(int index, NamedBox& box, string& aux_word)
{
	Params<XComplex> nearer = box.nearer();
	Params<XComplex> further = box.further();
	Params<XComplex> greater = box.greater();
	switch(index) {
		case 0: return check_bounds(absUB(further.loxodromic_sqrt) < 1.0);
		case 1: return check_bounds(greater.loxodromic_sqrt.im < 0.0
                                 || greater.lattice.im < 0.0
                                 || greater.parabolic.im < 0.0
                                 || greater.parabolic.re < 0.0);
		case 2: return check_bounds(fabs(nearer.lattice.re) > 0.5);
		case 3: return check_bounds(absUB(further.lattice) < 1.0);
        // Note: we can exclude the box if and only if the parabolic imag part is
        // bigger than half the lattice imag part over the WHOLE box
        // We assume that case 1 has been tested. Multiplication by 0.5 is EXACT (if no underflow or overflow)
		case 4: return check_bounds(nearer.parabolic.im > 0.5*further.lattice.im);
		case 5: return check_bounds(nearer.parabolic.re > 0.5);
		case 6: {
            // Area is |lox_sqrt|^2*|Im(lattice)|.
            double area = areaLB(nearer);
		    return check_bounds(area > g_maximumArea);
		}
		default: {
			Params<ACJ> cover(box.cover());
			box_state result = evaluate_ACJ(indexString[index-7], cover, aux_word);
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


int TestCollection::add(string buf)
{
    size_t start = buf.find('(');   
    string word;
    if (start != string::npos) {
        size_t end = buf.find(')');
        word = buf.substr(start+1,end-start-1);
    } else {
        word = buf;
    }  
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
