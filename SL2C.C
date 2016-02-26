/*57:*/
#line 152 "glue.w"

#include "SL2C.h"
const SL2C operator*(const SL2C&x,const SL2C&y)
{/*24:*/
#line 21 "SL2C.w"

return SL2C(
(x.a*y.a+x.b*y.c).z,(x.a*y.b+x.b*y.d).z,
(x.c*y.a+x.d*y.c).z,(x.c*y.b+x.d*y.d).z
);

/*:24*/
#line 155 "glue.w"
}
const SL2C inverse(const SL2C&x)
{/*25:*/
#line 31 "SL2C.w"

return SL2C(x.d,-x.b,-x.c,x.a);
#line 1 "ACJ.w"
/*:25*/
#line 157 "glue.w"
}

/*:57*/
