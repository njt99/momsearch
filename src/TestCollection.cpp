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
// 7. all slopes of length at most 6 have distance at most 5
// 8. we are in the var_nbd of two quasi-relators
// 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "TestCollection.h"
#include "CanonicalName.h"
#include "slopes.h"
#include "ImpossibleRelations.h"
#include <algorithm>
using namespace std;

double g_maximumArea = 6.0;
double g_minimumArea = 0.0;
int g_max_g_len = 7;
int g_num_bnd_tests = 9;
int g_var_int_depth = 36; // don't look for var_int unles we are this deep
int g_slope_dist_depth = 36; 

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
	return g_num_bnd_tests + indexString.size();
}

SL2ACJ TestCollection::construct_word_simple(string word, const Params<ACJ>& params)
{
	ACJ one(1.), zero(0.);
	SL2ACJ w(one, zero, zero, one);
	SL2ACJ G(constructG(params));
	SL2ACJ g(inverse(G));

    char h;
	int M = 0;
	int N = 0;
    ACJ T;
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
                    T = params.lattice * double(N) + double(M);
                    w = SL2ACJ(w.a + T * w.c, w.b + T * w.d, w.c, w.d);
					M = N = 0;
				}
				if (h == 'g')
					w = g * w;
				else if (h == 'G')
					w = G * w;
			}
		}
	}
    if (M != 0 || N != 0) {
        T = params.lattice * double(N) + double(M);
        w = SL2ACJ(w.a + T * w.c, w.b + T * w.d, w.c, w.d);
    }
    return w;
}

SL2C TestCollection::construct_word(string word, const Params<XComplex>& params)
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

SL2ACJ TestCollection::construct_word(string word, const Params<ACJ>& params, 
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


box_state TestCollection::is_var_intersection(Box& box, string& aux_word,
                                       unordered_map<int,ACJ>& para_cache, unordered_map<string,SL2ACJ>& words_cache)
{
    // TODO: Check this does what is should. Should show that two varieties intersect in box
    if (box.qr.words().size() < 2 || box.name.length() < g_var_int_depth) {
         if (box.qr.words().size() == 0) { return open; } 
         else { return open_with_qr; }
    }
	Params<ACJ> params = box.cover();
    vector<string> qrs(box.qr.words());
    sort(qrs.begin(), qrs.end(), g_power_sort);
    int min_power = g_power(qrs.front());
    vector<string> var_words;
	for (auto it = qrs.begin(); it != qrs.end(); ++it) {
        // We avoid high g_power relators for optimization.
        // TODO: check that this does not need to be relaxed when pushing for higher powers
        if (g_power(*it) > min_power + 1) {
            break;
        } else {
            SL2ACJ w = construct_word(*it, params, para_cache, words_cache);
            if (inside_var_nbd(w)) {
                var_words.push_back(*it);
            }
        }
    }
    if (var_words.size() > 1) {
	    for (auto it0 = var_words.begin(); it0 != var_words.end(); ++it0) {
	        for (auto it1 = ++(var_words.begin()); it1 != var_words.end(); ++it1) {
                // it is possible that the two words are related as qrs are not
                // always reprented by canonical names, especially given shifting,
                // so we check here. note, it is likely a no-op in practice given 
                // the way the word list in generated currently
                CanonicalName canonicalName;
                canonicalName.addRelator(*it0);
                // note, since g power increase, we only need to check one direction
                string can1 = canonicalName.getCanonicalName(*it1); 
                if (g_power(can1) == 0) { continue; }
                SL2ACJ w0 = construct_word(*it0, params, para_cache, words_cache); 
                SL2ACJ w1 = construct_word(*it1, params, para_cache, words_cache);
                // we want a point to be on the intersection 
                if (notZero(w1 - w0) && notZero(w1 + w0)) {
                    continue;
                } else {
                    string two_relators = *it0 + "," + *it1;
                    aux_word.assign(two_relators); 
                    return two_var_inter; 
                }
            }
        }
    }
    return open_with_qr;
}

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
                    //SL2ACJ w_sub = construct_word_simple(*it, params);
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
                            lookup_para = para_cache.emplace(4096*(-M)-N, ACJ());
                            if (lookup_para.second) {
                                ACJ T = params.lattice*double(-N) + double(-M);
                                swap(lookup_para.first->second, T);
                            }
                            w_k = SL2ACJ(w.a + lookup_para.first->second * w.c, w.b + lookup_para.first->second * w.d, w.c, w.d); // Cheaper multiplying

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
			return check_bounds_center(g_latticeArea > g_maximumArea || g_latticeArea < g_minimumArea);
		}
		case 7: {
            if (box.name.length() < g_slope_dist_depth) {
                return open;
            } 
            int max_dist = short_slopes_max_dist_center(box);
            return check_bounds_center(max_dist > 0 && max_dist < 6);
		}
		case 8: {
            if (box.qr.words().size() < 2 || box.name.length() < g_var_int_depth) {
                 if (box.qr.words().size() == 0) { return open; } 
                 else { return open_with_qr; }
            }
		    return two_var_center;
		}
		default:
			return evaluate_approx(indexString[index-g_num_bnd_tests], params);
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
            double area_lb = areaLB(nearer);
            double area_ub = areaUB(further);
		    return check_bounds(area_lb > g_maximumArea || area_ub < g_minimumArea );
		}
		case 7: {
            if (box.name.length() < g_slope_dist_depth) {
                return open;
            }
            int max_dist = short_slopes_max_dist(box);
            return check_bounds(max_dist > 0 && max_dist < 6);
		}
		case 8: {
            if (box.qr.words().size() < 2 || box.name.length() < g_var_int_depth) {
                 if (box.qr.words().size() == 0) { return open; } 
                 else { return open_with_qr; }
            }
		    return is_var_intersection(box, aux_word, para_cache, words_cache);
		}
		default: {
			Params<ACJ> cover(box.cover());
			box_state result = evaluate_ACJ(indexString[index-g_num_bnd_tests], cover, aux_word, new_qrs, para_cache, words_cache);
			return result;
		}
	}
}

// Returns the index number for the first basic g_num_bnd_tests tests
// or the quasi-relator if the index is g_num_bnd_tests or above
const char* TestCollection::getName(int index)
{
	static char buf[4];
	if (index < g_num_bnd_tests) {
		sprintf(buf, "%d", index);
		return buf;
	} else {
		return indexString[index-g_num_bnd_tests].c_str();
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
		stringIndex[word] = indexString.size();
		indexString.push_back(word);
		return indexString.size()+g_num_bnd_tests-1;
	}
	else
		return it->second+g_num_bnd_tests;
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

