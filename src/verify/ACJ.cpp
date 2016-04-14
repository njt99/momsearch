/*65:*/
#line 132 "glue.w"

#include "ACJ.h"
const ACJ operator*(const ACJ&x,const ACJ&y)
{/*30:*/
#line 97 "ACJ.w"

double xdist= size(x);
double ydist= size(y);
double ax= absUB(x.f),ay= absUB(y.f);
AComplex r_f= x.f*y.f;
AComplex r_f0= x.f*y.f0+x.f0*y.f;
AComplex r_f1= x.f*y.f1+x.f1*y.f;
AComplex r_f2= x.f*y.f2+x.f2*y.f;
double A= (xdist+x.e)*(ydist+y.e);
double B= ax*y.e+ay*x.e;
double C= (r_f.e+r_f0.e)+(r_f1.e+r_f2.e);
double r_error= (1+3*EPS)*(A+(B+C));
return ACJ(r_f.z,r_f0.z,r_f1.z,r_f2.z,r_error);

/*:30*/
#line 135 "glue.w"
}
const ACJ operator/(const ACJ&x,const ACJ&y)
{/*32:*/
#line 130 "ACJ.w"

double xdist= size(x);
double ydist= size(y);
double ax= absUB(x.f),ay= absLB(y.f);
double D= ay-(1+EPS)*(y.e+ydist);
if(!(D> 0))return ACJ(0,0,0,0,infinity());
AComplex den= (y.f*y.f);
AComplex r_f= x.f/y.f;
AComplex r_f0= (x.f0*y.f-x.f*y.f0)/den;
AComplex r_f1= (x.f1*y.f-x.f*y.f1)/den;
AComplex r_f2= (x.f2*y.f-x.f*y.f2)/den;
double A= (ax+(xdist+x.e))/D;
double B= (ax/ay+xdist/ay)+(ydist*ax)/(ay*ay);
double C= (r_f.e+r_f0.e)+(r_f1.e+r_f2.e);
double r_error= (1+3*EPS)*(((1+3*EPS)*A-(1-3*EPS)*B)+C);
return ACJ(r_f.z,r_f0.z,r_f1.z,r_f2.z,r_error);

/*:32*/
#line 137 "glue.w"
}
const ACJ operator/(double x,const ACJ&y)
{/*33:*/
#line 151 "ACJ.w"

double ydist= size(y);
double ax= fabs(x),ay= absLB(y.f);
double D= ay-(1+EPS)*(y.e+ydist);
if(!(D> 0))return ACJ(0,0,0,0,infinity());
AComplex den= (y.f*y.f);
AComplex r_f= x/y.f;
AComplex r_f0= (-x*y.f0)/den;
AComplex r_f1= (-x*y.f1)/den;
AComplex r_f2= (-x*y.f2)/den;
double B= ax/ay+(ydist*ax)/(ay*ay);
double C= (r_f.e+r_f0.e)+(r_f1.e+r_f2.e);
double r_error= (1+3*EPS)*(((1+2*EPS)*(ax/D)-(1-3*EPS)*B)+C);
return ACJ(r_f.z,r_f0.z,r_f1.z,r_f2.z,r_error);

/*:33*/
#line 139 "glue.w"
}
const ACJ sqrt(const ACJ&x)
{/*35:*/
#line 185 "ACJ.w"

double xdist= size(x);
double ax= absUB(x.f);
double D= ax-(1+EPS)*(xdist+x.e);
if(!(D> 0)){
return ACJ(0,0,0,0,(1+2*EPS)*sqrt(ax+(xdist+x.e)));
}else{
AComplex r_f= sqrt(x.f);
AComplex t= r_f+r_f;
AComplex r_f0= AComplex(x.f0.re,x.f0.im,0)/t;
AComplex r_f1= AComplex(x.f1.re,x.f1.im,0)/t;
AComplex r_f2= AComplex(x.f2.re,x.f2.im,0)/t;
double r_error= (1+3*EPS)*(
((1+EPS)*sqrt(ax)
-(1-3*EPS)*(xdist/(2*sqrt(ax))+sqrt(D)))
+((r_f.e+r_f0.e)+(r_f1.e+r_f2.e))
);
return ACJ(r_f.z,r_f0.z,r_f1.z,r_f2.z,r_error);
}

/*:35*/
#line 141 "glue.w"
}

/*:65*/
