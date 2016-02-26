/*48:*/
#line 6 "glue.w"

#ifndef _roundoff_h_
#define _roundoff_h_
#include <float.h> 
#include <math.h> 
/*1:*/
#line 40 "roundoff.w"

#define EPS DBL_EPSILON
#define HALFEPS (EPS/2)

/*:1*/
#line 11 "glue.w"

/*2:*/
#line 48 "roundoff.w"

inline double fabs(double x){return x<0?-x:x;}
void initialize_roundoff();
int roundoff_ok();
#ifndef __sparc__
inline double infinity(){return 1.0/0.0;}
#endif 

/*:2*/
#line 12 "glue.w"

#endif

/*:48*/
