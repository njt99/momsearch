/*67:*/
#line 158 "glue.w"

#include "SL2ACJ.h"
const SL2ACJ operator*(const SL2ACJ&x,const SL2ACJ&y)
{/*16:*/
#line 22 "SL2ACJ.w"

return SL2ACJ(
x.a*y.a+x.b*y.c,x.a*y.b+x.b*y.d,
x.c*y.a+x.d*y.c,x.c*y.b+x.d*y.d
);

/*:16*/
#line 161 "glue.w"
}
const SL2ACJ inverse(const SL2ACJ&x)
{/*17:*/
#line 32 "SL2ACJ.w"

return SL2ACJ(x.d,-x.b,-x.c,x.a);

/*:17*/
#line 163 "glue.w"
}
const ACJ orthodist(const SL2ACJ&x)
{/*18:*/
#line 39 "SL2ACJ.w"

ACJ t= x.a*x.d+x.b*x.c;
ACJ r= ACJ(sqrt(t*t-1));
ACJ r1= t+r;
if(r1.f.re*r1.f.re+r1.f.im*r1.f.im>=1)
return t+r;
else
return t-r;

/*:18*/
#line 165 "glue.w"
}
const ACJ length(const SL2ACJ&x)
{/*19:*/
#line 52 "SL2ACJ.w"

ACJ t= (x.a+x.d)*0.5;
ACJ r= ACJ(sqrt(t*t-1));
ACJ r1= t+r;
if(r1.f.re*r1.f.re+r1.f.im*r1.f.im>=1)
return(t+r)*(t+r);
else
return(t-r)*(t-r);

/*:19*/
#line 167 "glue.w"
}
const int notIdentity(const SL2ACJ&x)
{/*20:*/
#line 65 "SL2ACJ.w"

return absLB(x.b)> 0
||absLB(x.c)> 0
||(absLB(x.a-1)> 0&&absLB(x.a+1)> 0)
||(absLB(x.d-1)> 0&&absLB(x.d+1)> 0);

/*:20*/
#line 169 "glue.w"
}
const int notFPower(const SL2ACJ&x)
{/*21:*/
#line 77 "SL2ACJ.w"

return absLB(x.b)> 0||absLB(x.c)> 0;

/*:21*/
#line 171 "glue.w"
}
const SL2ACJ shortGenerator(const ACJ&z)
{/*22:*/
#line 84 "SL2ACJ.w"

ACJ sz= sqrt(z);
ACJ zero(0);
return SL2ACJ(sz,zero,zero,1/sz);


/*:22*/
#line 173 "glue.w"
}
const SL2ACJ closeGenerator(const ACJ&x,const ACJ&z)
{/*23:*/
#line 94 "SL2ACJ.w"

ACJ sx= sqrt(x),sz= sqrt(z);
ACJ sh= (sx-1/sx)*0.5;
ACJ ch= (sx+1/sx)*0.5;
return SL2ACJ(ch*sz,sh/sz,sh*sz,ch/sz);

#line 1 "ACJ.w"
/*:23*/
#line 175 "glue.w"
}


/*:67*/
