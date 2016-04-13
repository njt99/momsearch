/*
 *  Params.h
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */
#ifndef __Params_h
#define __Params_h
#include <math.h>
#include "SL2C.h"
#include "SL2ACJ.h"
#include <string>

template<class N> struct Params {
	N lattice;
	N loxodromicSqrt;
	N parabolic;
};

SL2C constructG(const Params<XComplex>& params);
SL2C constructT(const Params<XComplex>& params, int x, int y);
SL2ACJ constructG(const Params<ACJ>& params);
SL2ACJ constructT(const Params<ACJ>& params, int x, int y);

int g_power(std::string w);
bool g_power_sort(std::string a, std::string b);

// ULP functions
// TODO: CHECK FOR OVERLOW AND UNDERFLOW
// TODO: Perhaps we should utilize the same bounds for doubles
//       as are stated in the Annals paper instead of using nextafter
inline double inc_d(double x) { return nextafter(x,x+1); }
inline double dec_d(double x) {return nextafter(x,x-1); }
#endif // __Params_h
