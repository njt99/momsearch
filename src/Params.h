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
	N loxodromic_sqrt;
	N parabolic;
};

const SL2C constructG(const Params<XComplex>& params);
const SL2C constructT(const Params<XComplex>& params, int M, int N);
const SL2ACJ constructG(const Params<ACJ>& params);
const SL2ACJ constructT(const Params<ACJ>& params, int M, int N);

int g_power(std::string w);
bool g_power_sort(std::string a, std::string b);

#endif // __Params_h
