/*
 *  Params.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */

#include "SL2C.h"
#include "SL2ACJ.h"
#include "Params.h"

// the crucial map taking B_0 to B_infinity (and taking
// B_infinity to B_1') is of the form
// g(z) = p + 1 / (s^2 z)
//

SL2C constructG(const Params<XComplex>& params)
{
	XComplex I(0., 1.);
	const XComplex& sl = params.loxodromicSqrt;
	return SL2C((I*(params.parabolic*sl).z).z, (I/sl).z, (I*sl).z, XComplex(0.));
}

SL2C constructT(const Params<XComplex>& params, int x, int y)
{
	return SL2C(XComplex(1.), (params.lattice*double(y)+double(x)).z, XComplex(0.,0.), XComplex(1.,0.));
}

SL2ACJ constructG(const Params<ACJ>& params)
{
	ACJ I(XComplex(0., 1.));
	const ACJ& sl = params.loxodromicSqrt;
	return SL2ACJ(I*params.parabolic*sl, I/sl, I*sl, ACJ(0.));
}

SL2ACJ constructT(const Params<ACJ>& params, int x, int y)
{
	return SL2ACJ(ACJ(XComplex(1.)), params.lattice*ACJ(double(y))+double(x), ACJ(XComplex(0.)), ACJ(XComplex(1.)));
}

int g_power(std::string w) {
    int count = 0;
    for (std::string::size_type p = 0; p < w.size(); ++p) {
        if (w[p] == 'g' || w[p] == 'G') ++count;
    }
    return count;
} 

bool g_power_sort(std::string a, std::string b) { return g_power(a) < g_power(b); }

