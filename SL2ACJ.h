/*55:*/
#line 159 "glue.w"

#ifndef _SL2ACJ_h_
#define _SL2ACJ_h_
#include "ACJ.h"
/*41:*/
#line 10 "SL2ACJ.w"

struct SL2ACJ{
SL2ACJ():a(1),b(0),c(0),d(1){}
SL2ACJ(const ACJ&aa,const ACJ&bb,const ACJ&cc,const ACJ&dd)
:a(aa),b(bb),c(cc),d(dd){}
ACJ a,b,c,d;
};

/*:41*/
#line 163 "glue.w"

const SL2ACJ operator*(const SL2ACJ&x,const SL2ACJ&y);
const SL2ACJ inverse(const SL2ACJ&x);
const int notIdentity(const SL2ACJ&x);
#endif

/*:55*/
