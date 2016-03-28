/*56:*/
#line 143 "glue.w"

#ifndef _SL2C_h_
#define _SL2C_h_
#include "Complex.h"
/*23:*/
#line 10 "SL2C.w"

struct SL2C{
SL2C():a(1),b(0),c(0),d(1){}
SL2C(const XComplex&aa,const XComplex&bb,const XComplex&cc,const XComplex&dd)
:a(aa),b(bb),c(cc),d(dd){}
XComplex a,b,c,d;
};

/*:23*/
#line 147 "glue.w"

const SL2C operator*(const SL2C&x,const SL2C&y);
const SL2C inverse(const SL2C&x);
#endif

/*:56*/
