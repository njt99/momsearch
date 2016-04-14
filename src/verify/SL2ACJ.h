/*66:*/
#line 143 "glue.w"

#ifndef _SL2ACJ_h_
#define _SL2ACJ_h_
#include "ACJ.h"
/*15:*/
#line 10 "SL2ACJ.w"

struct SL2ACJ{
SL2ACJ():a(1),b(0),c(0),d(1){}
SL2ACJ(const ACJ&aa,const ACJ&bb,const ACJ&cc,const ACJ&dd)
:a(aa),b(bb),c(cc),d(dd){}
ACJ a,b,c,d;
};

/*:15*/
#line 147 "glue.w"

const SL2ACJ operator*(const SL2ACJ&x,const SL2ACJ&y);
const SL2ACJ inverse(const SL2ACJ&x);
const ACJ orthodist(const SL2ACJ&x);
const ACJ length(const SL2ACJ&x);
const int notIdentity(const SL2ACJ&x);
const int notFPower(const SL2ACJ&x);
const SL2ACJ shortGenerator(const ACJ&z);
const SL2ACJ closeGenerator(const ACJ&x,const ACJ&z);
#endif

/*:66*/
