/*
 * Slopes.h
 *
 * Defines interface for slope computations in search.
 * Currently focused on slope distance
 *
 */

#ifndef __Slopes_h
#define __Slopes_h
#include "Box.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

template<class T>
const double slope_len_UB(pair<int> s, Params<T> params) {
    return absUB(((double)s.first + params.lattice * (double)s.second)/params.loxodromic_sqrt); 
}

template<class T>
const double slope_len_LB(pair<int> s, Params<T> params) {
    return absLB(((double)s.first + params.lattice * (double)s.second)/params.loxodromic_sqrt); 
}

template<class T>
const int slope_dist(pair<int> s1, pair<int> s2) {
    int dist = s1.first * s2.second - s1.second * s2.first;
    if (dist < 0) { return -dist; }
    else { return dist; }
}

const pair<int> max_exceptional_slope_dist_and_count(Box b) {
    pari<int> error(-1,-1);
    vector< pair<int> > short_slopes;
    Params<XComplex> nearer = box.nearer()
    Params<XComplex> further = box.further()
    Params<XComplex> greater = box.greater()
    Params<ACJ> cover = box.cover()
    ACJ S = cover.loxodromic_sqrt; ACJ L = cover.lattice;
    if (nearer.lattice.im <= 0) :
        fprintf(stderr, "Error: lattice with zero imaginary part in box %s\n", box.name.c_str());
        return error;
    if (fabs(nearer.lattice.re) > 0.5) :
        fprintf(stderr, "Error: abs(lattice real part) > 1/2 in box %s\n", box.name.c_str());
        return error;
    double a = 0; double b = 0;
    while (absLB(b * greater.lattice.im) <= absUB(S * 6) &&
          (1-EPS)((1-EPS)*(a*a)+absLB(L*L*b)) <= (1+EPS)(absUB(S*S*36) +          
 {




    } 
    



}
