/*62:*/
#line 71 "glue.w"

#include "Complex.h"
AComplex operator/(const AComplex&x,const AComplex&y)
{/*54:*/
#line 150 "Complex.w"

double nrm= y.z.re*y.z.re+y.z.im*y.z.im;
double xryr= x.z.re*y.z.re;
double xiyi= x.z.im*y.z.im;
double xiyr= x.z.im*y.z.re;
double xryi= x.z.re*y.z.im;
assert(y.e*y.e<(10000*EPS*EPS)*nrm);
double A= (fabs(xryr)+fabs(xiyi))+(fabs(xiyr)+fabs(xryi));
double B= x.e*(fabs(y.z.re)+fabs(y.z.im))
+y.e*(fabs(x.z.re)+fabs(x.z.im));
double e= (1+4*EPS)*(((5*HALFEPS)*A+(1+103*EPS)*B)/nrm);
return AComplex((xryr+xiyi)/nrm,(xiyr-xryi)/nrm,e);

/*:54*/
#line 74 "glue.w"
}

AComplex operator/(const XComplex&x,const XComplex&y)
{/*53:*/
#line 135 "Complex.w"

double nrm= y.re*y.re+y.im*y.im;
double xryr= x.re*y.re;
double xiyi= x.im*y.im;
double xiyr= x.im*y.re;
double xryi= x.re*y.im;
double re= (xryr+xiyi)/nrm;
double im= (xiyr-xryi)/nrm;
double A= ((fabs(xryr)+fabs(xiyi))+(fabs(xiyr)+fabs(xryi)))/nrm;
double e= (5*HALFEPS)*((1+3*EPS)*A);
return AComplex(re,im,e);

/*:53*/
#line 77 "glue.w"
}

AComplex operator/(double x,const XComplex&y)
{/*52:*/
#line 124 "Complex.w"

double nrm= y.re*y.re+y.im*y.im;
double re= (x*y.re)/nrm;
double im= -(x*y.im)/nrm;
double e= (2*EPS)*((1+2*EPS)*(fabs(re)+fabs(im)));
return AComplex(re,im,e);

/*:52*/
#line 80 "glue.w"
}

AComplex sqrt(const XComplex&x)
{/*55:*/
#line 166 "Complex.w"

double s= sqrt((fabs(x.re)+hypot(x.re,x.im))*0.5);
double d= (x.im/s)*0.5;
double e= EPS*((1+4*EPS)*(1.25*s+1.75*fabs(d)));
if(x.re> 0.0)
return AComplex(s,d,e);
else
return AComplex(d,s,e);

/*:55*/
#line 83 "glue.w"
}

/*:62*/
