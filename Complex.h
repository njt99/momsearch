/*47:*/
#line 19 "glue.w"

#ifndef _Complex_h_
#define _Complex_h_
#include <assert.h> 
#include "roundoff.h"
/*5:*/
#line 11 "Complex.w"

struct XComplex{
double re;
double im;
XComplex(double r= 0,double i= 0):re(r),im(i){}
};

/*:5*/
#line 24 "glue.w"

/*6:*/
#line 21 "Complex.w"

struct AComplex{
XComplex z;
double e;
AComplex(double r= 0,double i= 0,double err= 0):z(r,i),e(err){}
};

/*:6*/
#line 25 "glue.w"

inline const XComplex operator-(const XComplex&x);
inline const AComplex operator+(const AComplex&x,const AComplex&y);
inline const AComplex operator+(const XComplex&x,const XComplex&y);
inline const AComplex operator+(const XComplex&x,double y);
inline const AComplex operator-(const AComplex&x,const AComplex&y);
inline const AComplex operator-(const XComplex&x,const XComplex&y);
inline const AComplex operator-(const XComplex&x,double y);
inline const AComplex operator*(const XComplex&x,const XComplex&y);
inline const AComplex operator*(const XComplex&x,double y);
inline const AComplex operator/(const XComplex&x,double y);
inline const double absLB(const XComplex&x);
inline const double absUB(const XComplex&x);
AComplex operator/(const AComplex&x,const AComplex&y);
AComplex operator/(const XComplex&x,const XComplex&y);
AComplex operator/(double x,const XComplex&y);
AComplex sqrt(const XComplex&x);
#include "Complex.inline"
#endif

/*:47*/
