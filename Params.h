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
#include "GL2C.h"
#include "GL2ACJ.h"

template<class N> struct Params {
	N lattice;
	N loxodromicSqrt;
	N parabolic;
};

GL2C constructG(const Params<Complex>& params);
GL2C constructT(const Params<Complex>& params, int x, int y);
GL2ACJ constructG(const Params<AComplex1Jet>& params);
GL2ACJ constructT(const Params<AComplex1Jet>& params, int x, int y);
#endif // __Params_h
