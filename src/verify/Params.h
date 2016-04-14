#ifndef __Params_h
#define __Params_h
#include <math.h>
#include "SL2ACJ.h"

template<class N> struct Params {
	N lattice;
	N loxodromicSqrt;
	N parabolic;
};

SL2ACJ construct_G(const Params<ACJ>& params);
SL2ACJ construct_T(const Params<ACJ>& params, int x, int y);
SL2ACJ construct_word(const Params<ACJ>& params, char* word);

// TODO: CHECK FOR OVERLOW AND UNDERFLOW
// TODO: Perhaps we should utilize the same bounds for doubles
//       as are stated in the Annals paper instead of using nextafter
inline double inc_d(double x) {return nextafter(x,x+1); }
inline double dec_d(double x) {return nextafter(x,x-1); }
#endif // __Params_h
