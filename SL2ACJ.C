/*59:*/
#line 172 "glue.w"

#include "SL2ACJ.h"
const SL2ACJ operator*(const SL2ACJ&x,const SL2ACJ&y)
{/*42:*/
#line 22 "SL2ACJ.w"

return SL2ACJ(
x.a*y.a+x.b*y.c,x.a*y.b+x.b*y.d,
x.c*y.a+x.d*y.c,x.c*y.b+x.d*y.d
);

/*:42*/
#line 175 "glue.w"
}
const SL2ACJ operator+(const SL2ACJ&x,const SL2ACJ&y)
{/*43:*/
#line 32 "SL2ACJ.w"

return SL2ACJ(
x.a+y.a,x.b+y.b,
x.c+y.c,x.d+y.d
);

/*:43*/
#line 177 "glue.w"
}
const SL2ACJ operator-(const SL2ACJ&x,const SL2ACJ&y)
{/*44:*/
#line 42 "SL2ACJ.w"

return SL2ACJ(
x.a-y.a,x.b-y.b,
x.c-y.c,x.d-y.d
);

/*:44*/
#line 179 "glue.w"
}
const SL2ACJ inverse(const SL2ACJ&x)
{/*45:*/
#line 52 "SL2ACJ.w"

return SL2ACJ(x.d,-x.b,-x.c,x.a);

/*:45*/
#line 181 "glue.w"
}
const int notIdentity(const SL2ACJ&x)
{/*46:*/
#line 59 "SL2ACJ.w"

return absLB(x.b)> 0
||absLB(x.c)> 0
||(absLB(x.a-1)> 0&&absLB(x.a+1)> 0)
||(absLB(x.d-1)> 0&&absLB(x.d+1)> 0);

/*:46*/
#line 183 "glue.w"
}
const int notZero(const SL2ACJ&x)
{/*47:*/
#line 69 "SL2ACJ.w"

return absLB(x.a)> 0
||absLB(x.b)> 0
||absLB(x.c)> 0
||absLB(x.d)> 0;
#line 1 "glue.w"
/*:47*/
#line 185 "glue.w"
}/*:59*/
