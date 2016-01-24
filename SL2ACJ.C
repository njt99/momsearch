/*56:*/
#line 169 "glue.w"

#include "SL2ACJ.h"
const SL2ACJ operator*(const SL2ACJ&x,const SL2ACJ&y)
{/*42:*/
#line 22 "SL2ACJ.w"

return SL2ACJ(
x.a*y.a+x.b*y.c,x.a*y.b+x.b*y.d,
x.c*y.a+x.d*y.c,x.c*y.b+x.d*y.d
);

/*:42*/
#line 172 "glue.w"
}
const SL2ACJ inverse(const SL2ACJ&x)
{/*43:*/
#line 32 "SL2ACJ.w"

return SL2ACJ(x.d,-x.b,-x.c,x.a);

/*:43*/
#line 174 "glue.w"
}
const int notIdentity(const SL2ACJ&x)
{/*44:*/
#line 39 "SL2ACJ.w"

return absLB(x.b)> 0
||absLB(x.c)> 0
||(absLB(x.a-1)> 0&&absLB(x.a+1)> 0)
||(absLB(x.d-1)> 0&&absLB(x.d+1)> 0);
#line 1 "glue.w"
/*:44*/
#line 176 "glue.w"
}/*:56*/
