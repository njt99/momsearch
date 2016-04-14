/*58:*/
#line 6 "glue.w"

#ifndef _roundoff_h_
#define _roundoff_h_
#include <float.h> 
#include <math.h> 
/*8:*/
#line 40 "roundoff.w"

#define EPS DBL_EPSILON
#define HALFEPS (EPS/2)

/*:8*/
#line 11 "glue.w"

/*9:*/
#line 48 "roundoff.w"

#ifdef __GNUG__
inline double fabs(double x){return x<0?-x:x;}
extern"C"{
#endif 
void initialize_roundoff();
int roundoff_ok();
#ifndef __sparc__
inline double infinity(){return 1.0/0.0;}
#endif 
#ifdef __GNUG__
}
#endif 

/*:9*/
#line 12 "glue.w"

#endif

/*:58*/
