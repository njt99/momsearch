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
#include <stdlib.h>

using namespace std;

typedef pair<int,int> slope;

template<class T>
const double slope_len_UB(slope s, Params<T> params) {
    return absUB(((double)s.first + params.lattice * (double)s.second)/params.loxodromic_sqrt); 
}

template<class T>
const double slope_len_LB(slope s, Params<T> params) {
    return absLB(((double)s.first + params.lattice * (double)s.second)/params.loxodromic_sqrt); 
}

#endif // __Slopes_h
