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
// 2. -1/2 <= Re(l) <= 1/2 (reduction modulo M)
// 3. Im(l >= 0 (negation)
// 4. 0 <= Im(p) <= Im(l)/2 (reduction modulo N, flipping sign of N)
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
int g_max_g_len = 7;

string g_testCollectionFullWord;

int g_length(string& w) 
{
    int g_count = 0;
    for (string::size_type pos = 0; pos < w.size(); ++pos)
        if (w[pos] == 'g' || w[pos] == 'G')
            ++g_count;
    return g_count;
}

int TestCollection::size()
{
	return 7 + indexString.size();
}

SL2C TestCollection::construct_word(string word, Params<XComplex>& params)
{
    SL2C w(1,0,0,1);
	SL2C G(constructG(params));
	SL2C g(inverse(G));

    char h;	
	int N = 0;
	int M = 0;
    string::reverse_iterator rit;
    for (rit = word.rbegin(); rit != word.rend(); ++rit) {
        h = *rit;
		switch(h) {
			case 'm': --M; break;
			case 'M': ++M; break;
			case 'n': --N; break;
			case 'N': ++N; break;
			default: {
				if (M != 0 || N != 0) {
					w = constructT(params, M, N) * w;
					M = N = 0;
				}
				if (h == 'g')
					w = g * w;
				else if (h == 'G')
					w = G * w;
			}
		}
	}
    // Any leading M,N's
    if (M != 0 || N != 0) w = constructT(params, M, N) * w;
    // Rounding errors are irrelevant here, only used to guide search.
	return w;
}

SL2ACJ TestCollection::construct_word(string word, Params<ACJ>& params, 
               unordered_map<int,ACJ>& para_cache, unordered_map<string,SL2ACJ>& words_cache)
{
    pair<unordered_map<string,SL2ACJ>::iterator,bool> lookup;
    pair<unordered_map<int,ACJ>::iterator,bool> lookup_para;
    if (words_cache.size() == 0) {
        ACJ one(1), zero(0);
        words_cache.emplace("", SL2ACJ(one,zero,zero,one));
        lookup = words_cache.emplace("G", constructG(params));
        words_cache.emplace("g", inverse(lookup.first->second));
    }

    // Check, just in case we've already seen this word
    lookup = words_cache.emplace(word, SL2ACJ());
    if (lookup.second) {// not found 
        words_cache.erase(lookup.first);
    }
    else {
        return lookup.first->second;
    }
    
	SL2ACJ* w = &words_cache[""];
	SL2ACJ* G = &words_cache["G"];
	SL2ACJ* g = &words_cache["g"];

    char h;
	int M = 0;
	int N = 0;
    int g_len = 0;
    ACJ T;
    SL2ACJ w_store;
    string::iterator it;
    for (it = word.end(); it != word.begin(); --it) {
        h = *(it-1);
		switch(h) {
			case 'm': --M; break;
			case 'M': ++M; break;
			case 'n': --N; break;
			case 'N': ++N; break;
			default: {
                if (g_len < 4) {
                    lookup = words_cache.emplace(string(it-1, word.end()), SL2ACJ());
                    if (lookup.second) { // tail of word was not found
                        if (M != 0 || N != 0) { // need to deal with parabolic
                            if (abs(M) > 1024 || abs(N) > 1024) { fprintf(stderr, "Error constructing word: huge translation\n"); }
                            lookup_para = para_cache.emplace(4096*M+N, ACJ());
                            if (lookup_para.second) {
                                T = params.lattice*double(N) + double(M);
                                swap(lookup_para.first->second, T);
                            }
                            w_store = SL2ACJ(w->a + lookup_para.first->second * w->c, w->b + lookup_para.first->second * w->d, w->c, w->d); // Cheaper multiplying
                            w = &w_store;
                        }
                        if (h == 'g') {
                            w_store = (*g) * (*w);
                        }
                        else if (h == 'G') {
                            w_store = (*G) * (*w);
                        }
                        else {
                            fprintf(stderr, "Error constructing word: %s. Unknown generator!\n", word.c_str());
                        }
                        swap(lookup.first->second, w_store);
                    }
                    w = &(lookup.first->second); 
                } else {
                    if (M != 0 || N != 0) { // need to deal with parabolic
                        if (abs(M) > 1024 || abs(N) > 1024) { fprintf(stderr, "Error constructing word: huge translation\n"); }
                        lookup_para = para_cache.emplace(4096*M+N, ACJ());
                        if (lookup_para.second) {
                            T = params.lattice*double(N) + double(M);
                            swap(lookup_para.first->second, T);
                        }
                        w_store = SL2ACJ(w->a + lookup_para.first->second * w->c, w->b + lookup_para.first->second * w->d, w->c, w->d); // Cheaper multiplying
                        w = &w_store;
                    }
                    if (h == 'g') {
                        w_store = (*g) * (*w);
                    }
                    else if (h == 'G') {
                        w_store = (*G) * (*w);
                    }
                    else {
                        fprintf(stderr, "Error constructing word: %s. Unknown generator!\n", word.c_str());
                    }
                    w = &w_store;
                }
                M = 0;
                N = 0;
                ++g_len;
            }
		}
	}
    // Any leading M,N's
    if (M != 0 || N != 0) { // need to deal with parabolic
        if (abs(M) > 1024 || abs(N) > 1024) { fprintf(stderr, "Error constructing word: huge translation\n"); }
        lookup_para = para_cache.emplace(4096*M+N, ACJ());
        if (lookup_para.second) {
            T = params.lattice*double(N) + double(M);
            swap(lookup_para.first->second, T);
        }
        w_store = SL2ACJ(w->a + lookup_para.first->second * w->c, w->b + lookup_para.first->second * w->d, w->c, w->d); // Cheaper multiplying
        if (g_len < 5) {
            lookup = words_cache.emplace(word, w_store);
            if (!lookup.second) {
                fprintf(stderr, "Error constructing word: %s. It already exists but shouldn't!\n", word.c_str());
            } 
            w = &(lookup.first->second);
        } else {
            w = &w_store;
        } 
    }
//    fprintf(stderr, "Word: %s\n", word.c_str());
//    fprintf(stderr, "Box: %s\n", params.box_name.c_str());
//    fprintf(stderr, "a: %f + I %f with absLB  %f and absUB %f\n", w->a.f.re, w->a.f.im, absLB(w->a), absUB(w->a));
//    fprintf(stderr, "b: %f + I %f with absLB  %f and absUB %f\n", w->b.f.re, w->b.f.im, absLB(w->b), absUB(w->b));
//    fprintf(stderr, "c: %f + I %f with absLB  %f and absUB %f\n", w->c.f.re, w->c.f.im, absLB(w->c), absUB(w->c));
//    fprintf(stderr, "d: %f + I %f with absLB  %f and absUB %f\n", w->a.f.re, w->a.f.im, absLB(w->a), absUB(w->a));
    return *w;
}

/* Tests if the box is within the variety neighborhood for 
   its shortest g power quasi-relators. This test is used to 
   stop subdivision of a HOLE. The relevant Lemma:

{\bf Lemma (Variety Buffer-Zone Lemma):} {\it Let $W(p) =  (a\ \ b\ \ c\ \ d)$ be the matrix in ${\bf PSL}(2,{\bf C})$ determined by the variety word $W = W(m,n,g)$ at the point $p \in {\cal P}$, let $V_W$ be the variety determined by $W = I$ in ${\cal P}$, and let $N_W$ be the neighborhood of $V_W$  determined by the conditions that the absolute value of the $c$ co-ordinate of $W(p)$ is less than $1$ and (if the absolute value of the $c$ co-ordinate of $W(p)$ is zero) the absolute value of the $b$ coordinate is less than $1$.  Then no point in $N_W - V_W$ corresponds to a discrete, torsion-free hyperbolic 3-manifold.}

*/

//bool TestCollection::box_inside_nbd(Box& box, string& var_word)
//{
//    // Checks if a minimal g-power quasi relator pusts the box in a neighborhood.
//    // Returns true if found and copies word into var_word
//    Params<ACJ> params = box.cover();
//    vector<string> qrs = box.qr.wordClasses();
//    if (qrs.empty()) { return false; }
//    sort(qrs.begin(), qrs.end(), g_power_sort);
//    int min_power = g_power(qrs.front());
//	for (vector<string>::iterator it = qrs.begin(); it != qrs.end(); ++it) {
//        if (g_power(*it) > min_power) {
//            break;
//        } else {
//            SL2ACJ w = construct_word(*it, params);
//            if (inside_var_nbd(w)) {
//                var_word = *it;
//                return true;
//            } 
//        }
//    }
//    return false;
//}
//
//bool TestCollection::box_inside_at_least_two_nbd(Box& box, vector<string>& var_words)
//{
//    // Check if there are two quasi-relators that put the box in a neighborhood. 
//    // Returns true of two were found and places them in var_words.
//    Params<ACJ> params = box.cover();
//    vector<string> qrs = box.qr.wordClasses();
//    if (qrs.size() < 2) { return false; }
//    sort(qrs.begin(), qrs.end(), g_power_sort);
//    int min_power = g_power(qrs.front());
//    vector<string> found;
//	for (vector<string>::iterator it = qrs.begin(); it != qrs.end(); ++it) {
//        if (g_power(*it) > min_power && found.size() > 1) {
//            break;
//        } else {
//            SL2ACJ w = construct_word(*it, params);
//            if (inside_var_nbd(w)) {
//                found.push_back(*it);
//            } 
//        }
//    }
//    if (found.size() > 1) {
//        var_words.swap(found);
//        return true;
//    }
//    return false; 
//}


//bool TestCollection::valid_identity_cyclic(string word, Params<ACJ>& params)
//{
//    // Checks to see is ALL  cyclic permutations of a word is identity somewhere in the box
//    if (!word.size()) return false;
//	for (string::size_type pos = 0; pos < word.size(); ++pos) {
//		string pword = word.substr(pos, word.size()-pos) + word.substr(0, pos);
//		SL2ACJ w = construct_word(pword, params);
//		if (not_identity(w)) {
//			return false;
////        } else {
////            XComplex a = w.a.center();
////            XComplex b = w.b.center();
////            XComplex c = w.c.center();
////            XComplex d = w.d.center();
////            fprintf(stderr, "This word is identity somewhere in the box\n");
////            fprintf(stderr, "Word: %s\n", word.c_str());
////            fprintf(stderr, "At the center is has coords\n");
////            fprintf(stderr, "a: %f + I %f\n", a.re, a.im);
////            fprintf(stderr, "b: %f + I %f\n", b.re, b.im);
////            fprintf(stderr, "c: %f + I %f\n", c.re, c.im);
////            fprintf(stderr, "d: %f + I %f\n", d.re, d.im);
//        }
//	}
//	return true;
//}

//bool TestCollection::valid_variety(string word, Params<ACJ>& params)
//{
//    // Checks to see is ALL  cyclic box is small enough for all cyclic permutations to be inside variety neighborhood
//    if (!word.size()) return false;
//	for (string::size_type pos = 0; pos < word.size(); ++pos) {
//            string pword = word.substr(pos, word.size()-pos) + word.substr(0, pos);
//            SL2ACJ w = construct_word(pword, params);
//            if (!(absUB(w.c) < 1 && absUB(w.b) < 1)) { return false; }
//	}
//	return true;
//}
//
//bool TestCollection::valid_intersection(Box& box) {
//    // TODO: Check this does what is should. Should show that two varieties intersect in box
//	Params<ACJ> params = box.cover();
//    vector<string> qrs(box.qr.wordClasses());
//    if (qrs.size() < 2) { return false; }
//    sort(qrs.begin(), qrs.end(), g_power_sort);
//    int min_power = g_power(qrs.front());
//    int found = 0;
//    vector<string> var_jets;
//	for (vector<string>::iterator it = qrs.begin(); it != qrs.end(); ++it) {
//        if (g_power(*it) > min_power && found > 1) {
//            break;
//        } else {
//            if (!valid_variety(*it, params)) { return false; }
//            var_jets.push_back(*it);
//            found += 1;
//        }
//    }
//    if (found > 1) {
//        string w0 = var_jets[0];
//        string w1 = var_jets[1];
//        for (string::size_type idx_0 = 0; idx_0 < w0.size(); ++idx_0) {
//            string w0_p = w0.substr(idx_0, w0.size()-idx_0) + w0.substr(0, idx_0);
//            SL2ACJ w0_j(construct_word(w0_p, params));
//            for (string::size_type idx_1 = 0; idx_1 < w1.size(); ++idx_1) {
//                string w1_p = w1.substr(idx_1, w1.size()-idx_1) + w1.substr(0, idx_1);
//                SL2ACJ w1_j(construct_word(w1_p, params));
//                if (notZero(w1_j - w0_j) && notZero(w1_j + w0_j)) { return false; }
//            }
//        }
//        return true;
//    } else {
//        return false;
//    }
//}

box_state TestCollection::evaluate_approx(string word, Params<XComplex>& params)
{
    SL2C w = construct_word(word, params);
    if (maybe_variety(w) ) return variety_center;
    if (!maybe_large_horoball(w,params)) return open;
    return large_horoball_center;
}

bool TestCollection::only_bad_parabolics(SL2ACJ& w, Params<ACJ>& params)
{
    // Tests if w hits any lattice points (when w is parabolic).
    // This test is inconclusive is w has large transtalion (i.e. translate
    // w's center into the first postive quad of the lattice.)

    // fprintf(stderr, "Bad Parabolics\n");

    double one = 1; // Exact
    // We check the box is small enough to determine the sign of translation
    if (!( absUB(w.d - one) < 2 || absUB(w.d + one) < 2 || 
           absUB(w.a - one) < 2 || absUB(w.a + one) < 2 )) { return false; }

    // fprintf(stderr, "Signs d-1: %f d+1: %f a-1: %f a+1: %f\n", absUB(w.d - one), absUB(w.d + one), absUB(w.a - one), absUB(w.d + one));

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

    ACJ T = (absUB(w.d - one) < 2 || absUB(w.a - one) < 2) ? w.b : -w.b;
    ACJ L = params.lattice;
    // fprintf(stderr, "T: %f + I %f with absLB  %f and absUB %f\n", T.f.re, T.f.im, absLB(T), absUB(T));
    // fprintf(stderr, "L: %f + I %f with absLB  %f and absUB %f\n", L.f.re, L.f.im, absLB(L), absUB(L));

    ACJ d1 = T / (L + one);
    // fprintf(stderr, "Test T/(L+1): %f\n", absUB(d1));
    if (absUB(d1) >= 1) { return false; }
    ACJ d2 = d1 - one; // uses fewer operations
    // fprintf(stderr, "Test T/(L+1) - 1: %f\n", absUB(d2));
    if (absUB(d2) >= 1) { return false; }
    ACJ d3 = (T - one) / (L - one);
    // fprintf(stderr, "Test (T-1)/(L-1): %f\n", absUB(d3));
    if (absUB(d3) >= 1) { return false; }
    ACJ d4 = d3 - one; // better error estimate
    // fprintf(stderr, "Test (T-1)/(L-1) - 1: %f\n", absUB(d4));
    if (absUB(d4) >= 1) { return false; }
    return true;
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

box_state TestCollection::evaluate_ACJ(string word, Params<ACJ>& params, string& aux_word, vector<string>& new_qrs,
                                       unordered_map<int,ACJ>& para_cache, unordered_map<string,SL2ACJ>& words_cache)
{
    box_state state = open;
    bool found_qrs = false;
    aux_word.assign(word);
    int g_len = g_length(word);
    double one = 1; // Exact
	SL2ACJ w = construct_word(word, params, para_cache, words_cache);

    if (inside_var_nbd(w)) { 
        if (g_len <= g_max_g_len) {
            return variety_nbd;
        } else {
            fprintf(stderr, "Box in large length word VAR nbd, word : %s box : %s\n", word.c_str(), params.box_name.c_str()); 
        }
    }

    if (large_horoball(w,params)) {

        if (not_para_fix_inf(w)) {

			return killed_no_parabolics;

		} else {

			vector<string> mandatory;
			bool isImpossible = impossible->isAlwaysImpossible(word, mandatory);

			if (isImpossible) return killed_parabolics_impossible;
            else if (mandatory.size() > 0) {
                for (vector<string>::iterator it = mandatory.begin(); it != mandatory.end(); ++it) {
                    SL2ACJ w_sub = construct_word(*it, params, para_cache, words_cache);
                    if (not_para_fix_inf(w_sub)) {
                        aux_word.assign(*it);
                        return killed_elliptic;
                    }
                }
            }
   
            // Look for lattice points. We guess at the center
            // No reason to look for a unique lattice point if w.b has large size
            ACJ L = params.lattice;
            if (absLB(L) > 2*w.b.size) {
                XComplex cL = L.f;
                XComplex cT = (absUB(w.d - one) < 2 || absUB(w.a - one) < 2) ? w.b.f : -w.b.f;
                // XComplex cT = (w.b.f/w.d.f).z;
                // We expect T to be near the lattice point M_pow + N_pow*L
                int N_pow = (int) round(cT.im / cL.im);
                int M_pow = (int) round((cT - (cL * N_pow).z).z.re);
                // fprintf(stderr,"cT: %f + I%f with N_pow: %d and M_pos: %d\n", cT.re, cT.im, N_pow, M_pow);
                // We look over 16 nearby lattice points
                int s[5] = {0,-1,1,-2,2};
                SL2ACJ w_k;
                ACJ T;
                pair<unordered_map<int,ACJ>::iterator,bool> lookup_para;
                int N, M;
                for (int i = 0; i < 5; ++i) {
                    N = N_pow + s[i];
                    for (int j = 0; j < 5; ++j) {
                        state = open;
                        M = M_pow + s[j];
                        if (N == 0 && M == 0) {
                            w_k = w;
                        } else {
                            if (abs(M) > 1024 || abs(N) > 1024) { fprintf(stderr, "Error constructing word: huge translation\n"); }
                            lookup_para = para_cache.emplace(4096*M+N, ACJ());
                            if (lookup_para.second) {
                                T = params.lattice*double(N) + double(M);
                                swap(lookup_para.first->second, T);
                            }
                            // Shift to "0"
                            w_k = SL2ACJ(w.a - lookup_para.first->second * w.c, w.b - lookup_para.first->second * w.d, w.c, w.d); // Cheaper multiplying
                            // What if we now have a variety word?
//                            string word_k = shifted_word(word, - M, - N);
//                            fprintf(stderr, "Shifted Word %s\n", word_k.c_str());
//                            fprintf(stderr, "a: %f + I %f with absLB  %f and absUB %f\n", w_k.a.f.re, w_k.a.f.im, absLB(w_k.a), absUB(w_k.a));
//                            fprintf(stderr, "b: %f + I %f with absLB  %f and absUB %f\n", w_k.b.f.re, w_k.b.f.im, absLB(w_k.b), absUB(w_k.b));
//                            fprintf(stderr, "c: %f + I %f with absLB  %f and absUB %f\n", w_k.c.f.re, w_k.c.f.im, absLB(w_k.c), absUB(w_k.c));
//                            fprintf(stderr, "d: %f + I %f with absLB  %f and absUB %f\n", w_k.a.f.re, w_k.a.f.im, absLB(w_k.a), absUB(w_k.a));
                            if (inside_var_nbd(w_k)) { 
                                if (g_len <= g_max_g_len) {
                                    state = variety_nbd;
                                    break;
                                } else {
                                    fprintf(stderr, "Box in large length word VAR nbd, word : %s N: %d M: %d box : %s\n", word.c_str(), N, M, params.box_name.c_str()); 
                                }
                            }
                            if (not_para_fix_inf(w_k)) {
                                state = killed_no_parabolics;
                                break;
                            }
                            if (absUB(w_k.b) < 1) {
                                isImpossible = impossible->isImpossible(word, M, N, mandatory);
                                if (isImpossible) {
                                    state = killed_identity_impossible;
                                    break;
                                }
                                // Mandaotry includes list of things that must be parabolic. If they are not parabolic
                                // anywhere in the box, we can kill the box
                                for (vector<string>::iterator it = mandatory.begin(); it != mandatory.end(); ++it) {
                                    SL2ACJ w_sub = construct_word(*it, params, para_cache, words_cache);
                                    if (not_para_fix_inf(w_sub)) {
                                        aux_word.assign(*it);
                                        return killed_elliptic;
                                    }
                                }
                            }
                        }
                        if (absUB(w_k.b) < 1 && absLB(w_k.b) > 0) {
    //                        string word_k = shifted_word(word, - M, - N);
    //                        fprintf(stderr, "Killed by Failed qr %s\n", word_k.c_str());
    //                        SL2ACJ new_w_k = construct_word(word_k, params);
    //                        SL2ACJ gah_k = constructT(params, - M, - N) * w;
    //                        fprintf(stderr," absLB(b) = %f\n absLB(c) = %f\n absLB(a-1) = %f\n absLB(d-1) = %f\n absLB(a+1) = %f\n absLB(d+1) = %f\n",
    //                                        absLB(w_k.b), absLB(w_k.c), absLB(w_k.a - 1.), absLB(w_k.d - 1.), absLB(w_k.a + 1.), absLB(w_k.d + 1.));
    //                        fprintf(stderr," absLB(b) = %f\n absLB(c) = %f\n absLB(a-1) = %f\n absLB(d-1) = %f\n absLB(a+1) = %f\n absLB(d+1) = %f\n",
    //                                        absLB(new_w_k.b), absLB(new_w_k.c), absLB(new_w_k.a - 1.), absLB(new_w_k.d - 1.), absLB(new_w_k.a + 1.), absLB(new_w_k.d + 1.));
    //                        fprintf(stderr," absLB(b) = %f\n absLB(c) = %f\n absLB(a-1) = %f\n absLB(d-1) = %f\n absLB(a+1) = %f\n absLB(d+1) = %f\n",
    //                                        absLB(gah_k.b), absLB(gah_k.c), absLB(gah_k.a - 1.), absLB(gah_k.d - 1.), absLB(gah_k.a + 1.), absLB(gah_k.d + 1.));
                            state = killed_failed_qr;
                            break;
                        }
                        if (only_bad_parabolics(w_k, params)) { // TODO: Test with constucted word!
                            // w_k is a bad parabolic
                            state = killed_bad_parabolic;
                            break;
                        }
                        if (absUB(w_k.b) < 1) {
                            // If nothing has worked, at least add w as a quasi relator
                            state = open_with_qr;
                            found_qrs = true;
                            new_qrs.push_back(shifted_word(word, - M, - N));
    //                        string word_k = shifted_word(word, - M, - N);
    //                        SL2ACJ new_w_k = construct_word(word_k, params);
    //                        fprintf(stderr,"Horo Ratio for new QR is %f\n", absUB(w_k.c / params.loxodromic_sqrt));
    //                        fprintf(stderr,"Reconstucted horo ratio for QR is %f\n", absUB(new_w_k.c / params.loxodromic_sqrt));
                        }
                    }
					if (state != open && state != open_with_qr) {
						aux_word.assign(shifted_word(word, - M, - N));
						return state;
					}
                }
            }
        }
    }
    if (found_qrs) return open_with_qr; 
    return open;
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

box_state TestCollection::evaluateBox(int index, Box& box, string& aux_word, vector<string>& new_qrs,
                                      unordered_map<int,ACJ>& para_cache, unordered_map<string,SL2ACJ>& words_cache)
{
	Params<XComplex> nearer = box.nearer();
	Params<XComplex> further = box.further();
	Params<XComplex> greater = box.greater();
//    if (index < 7) {
//        fprintf(stderr, "Evaluate for bounds of box %s\n", box.name.c_str());
//    }
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
            // fprintf(stderr, "Evaluate for word %s and box %s\n", indexString[index-7].c_str(), box.name.c_str());
			box_state result = evaluate_ACJ(indexString[index-7], cover, aux_word, new_qrs, para_cache, words_cache);
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
