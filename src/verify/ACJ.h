/*63:*/
#line 85 "glue.w"

#ifndef _ACJ_h_
#define _ACJ_h_
#include "Complex.h"
#include <assert.h> 
#include <stdio.h> 
#include "roundoff.h"
/*24:*/
#line 21 "ACJ.w"

struct ACJ{
ACJ(const XComplex&ff=0,const XComplex&ff0= 0,
const XComplex&ff1= 0,const XComplex&ff2= 0,double err= 0)
:f(ff),f0(ff0),f1(ff1),f2(ff2),e(err),
size((1+2*EPS)*(absUB(ff0)+(absUB(ff1)+absUB(ff2))))
{}

XComplex f;
XComplex f0;
XComplex f1;
XComplex f2;
double e;

double size;
};

/*:24*/
#line 92 "glue.w"

inline const ACJ operator-(const ACJ&x);
inline const ACJ operator+(const ACJ&x,const ACJ&y);
inline const ACJ operator-(const ACJ&x,const ACJ&y);
inline const ACJ operator+(const ACJ&x,double y);
inline const ACJ operator-(const ACJ&x,const ACJ&y);
inline const ACJ operator*(const ACJ&x,double y);
inline const ACJ operator/(const ACJ&x,double y);
inline const double absUB(const ACJ&x);
inline const double absLB(const ACJ&x);
inline const double size(const ACJ&x);
const ACJ operator*(const ACJ&x,const ACJ&y);
const ACJ operator/(const ACJ&x,const ACJ&y);
const ACJ operator/(double x,const ACJ&y);
const ACJ sqrt(const ACJ&x);
#include "ACJ.inline"
#endif

/*:63*/
