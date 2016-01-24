@*Complex numbers.
This section implements complex numbers.
We need two kinds: |XComplex|,
for complex numbers which are represented exactly,
and |AComplex|, which is an interval that contains the number we wish
to represent.
Most of the operations here act on |XComplex| and produce |AComplex|.

@ An |XComplex| $x$
represents the complex number $\reps{|x|} = \reps{|x.re|} + i \reps{|x.im|}$.
@<Definition of |XComplex|@>=
struct XComplex {
  double re;
  double im;
  XComplex(double r=0, double i=0) :re(r), im(i) {}
};

@ An |AComplex| $x$ represents the set of complex numbers
$\reps{|x|} = \{y : \abs{y-x.z} \le x.e\}$.

@<Definition of |AComplex|@>=
struct AComplex {
  XComplex z;
  double e;
  AComplex(double r, double i, double err) :z(r,i), e(err) {}
};

@ \proposition{-X}
If |x| is |XComplex|, then
$\reps{|-x|} = -\reps{|x|}$.
\endproposition
@<Definition of |-x| for |XComplex x|@>=
  return XComplex(-x.re, -x.im);

@ \proposition{X+d}
If |x| is |XComplex| and |y| is |double|, then
$\reps{|x+y|} \supset \reps{|x|} + \reps{|y|}$.
@<Definition of |x+y| for |XComplex x| and |double y|@>=
  double re = x.re + y;
  double e = HALFEPS*fabs(re);
  return AComplex(re, x.im, e);

@ \proposition{X-d}
If |x| is |XComplex| and |y| is |double|, then
$\reps{|x-y|} \supset \reps{|x|} - \reps{|y|}$.
\endproposition
@<Definition of |x-y| for |XComplex x| and |double y|@>=
  double re = x.re - y;
  double e = HALFEPS*fabs(re);
  return AComplex(re, x.im, e);

@ \proposition{X+X}
If |x| and |y| are |XComplex|, then
$\reps{|x+y|} \supset \reps{|x|} + \reps{|y|}$.
\endproposition
@<Definition of |x+y| for |XComplex x,y|@>=
  double re = x.re + y.re;
  double im = x.im + y.im;
  double e = HALFEPS*((1+EPS)*(fabs(re)+fabs(im)));
  return AComplex(re, im, e );

@ \proposition{X-X}
If |x| and |y| are |XComplex|, then
$\reps{|x-y|} \supset \reps{|x|} - \reps{|y|}$.
\endproposition
@<Definition of |x-y| for |XComplex x,y|@>=
  double re = x.re - y.re;
  double im = x.im - y.im;
  double e = HALFEPS*((1+EPS)*(fabs(re)+fabs(im)));
  return AComplex(re, im, e );

@ \proposition{A+A}
If |x| and |y| are |AComplex|, then
$\reps{|x+y|} \supset \reps{|x|} + \reps{|y|}$.
\endproposition
@<Definition of |x+y| for |AComplex x,y|@>=
  double re = x.z.re + y.z.re;
  double im = x.z.im + y.z.im;
  double e = (1+2*EPS)*(HALFEPS*(fabs(re)+fabs(im)) + (x.e + y.e));
  return AComplex(re,im,e);

@ \proposition{A-A}
If |x| and |y| are |AComplex|, then
$\reps{|x-y|} \supset \reps{|x|} - \reps{|y|}$.
\endproposition
@<Definition of |x-y| for |AComplex x,y|@>=
  double re = x.z.re - y.z.re;
  double im = x.z.im - y.z.im;
  double e = (1+2*EPS)*(HALFEPS*(fabs(re)+fabs(im)) + (x.e + y.e));
  return AComplex(re,im,e);

@ \proposition{X*d}
If |x| is |XComplex| and |y| is |double|, then
$\reps{|x*y|} \supset \reps{|x|} \reps{|y|}$.
\endproposition
@<Definition of |x*y| for |XComplex x| and |double y|@>=
  double re = x.re*y;
  double im = x.im*y;
  return AComplex(re, im, HALFEPS*((1+EPS)*(fabs(re)+fabs(im))) );

@ \proposition{X/d}
If |x| is |XComplex| and |y| is |double|, then
$\reps{|x/y|} \supset \reps{|x|} / \reps{|y|}$.
\endproposition
@<Definition of |x/y| for |XComplex x| and |double y|@>=
  double re = x.re/y;
  double im = x.im/y;
  return AComplex(re, im, HALFEPS*((1+EPS)*(fabs(re)+fabs(im))) );

@ \proposition{X*X}
If |x| and |y| are |XComplex|, then
$\reps{|x*y|} \supset \reps{|x|} \reps{|y|}$.
\endproposition
@<Definition of |x*y| for |XComplex x,y|@>=
  double re1 = x.re * y.re, re2 = x.im * y.im;
  double im1 = x.re * y.im, im2 = x.im * y.re;
  double e = EPS*((1+2*EPS)*((fabs(re1)+fabs(re2))+(fabs(im1)+fabs(im2))));
  return AComplex(re1-re2, im1+im2, e);

@ \proposition{d/X}
If |x| is |double| and |y| is |XComplex|, then
$\reps{|x/y|} \supset \reps{|x|} / \reps{|y|}$.
\endproposition
@<Definition of |x/y| for |double x| and |XComplex y|@>=
  double nrm = y.re * y.re + y.im * y.im;
  double re = (x * y.re) / nrm;
  double im = -(x * y.im) / nrm;
  double e = (2*EPS)*((1+2*EPS)*(fabs(re)+fabs(im)));
  return AComplex(re, im, e);

@ \proposition{X/X}
If |x| and |y| are |XComplex|, then
$\reps{|x/y|} \supset \reps{|x|} / \reps{|y|}$.
\endproposition
@<Definition of |x/y| for |XComplex x,y|@>=
  double nrm = y.re * y.re + y.im * y.im;
  double xryr = x.re*y.re;
  double xiyi = x.im*y.im;
  double xiyr = x.im*y.re;
  double xryi = x.re*y.im;
  double re = (xryr+xiyi)/nrm;
  double im = (xiyr-xryi)/nrm;
  double A = ((fabs(xryr)+fabs(xiyi))+(fabs(xiyr)+fabs(xryi)))/nrm;
  double e = (5*HALFEPS)*((1+3*EPS)*A);
  return AComplex(re, im, e);

@ \proposition{A/A}
If |x| and |y| are |AComplex|, then
$\reps{|x/y|} \supset \reps{|x|} / \reps{|y|}$.
@<Definition of |x/y| for |AComplex x,y|@>=
  double nrm = y.z.re * y.z.re + y.z.im * y.z.im;
  double xryr = x.z.re*y.z.re;
  double xiyi = x.z.im*y.z.im;
  double xiyr = x.z.im*y.z.re;
  double xryi = x.z.re*y.z.im;
  assert(y.e*y.e < (10000*EPS*EPS)*nrm);
  double A = (fabs(xryr)+fabs(xiyi))+(fabs(xiyr)+fabs(xryi));
  double B = x.e * (fabs(y.z.re) + fabs(y.z.im))
	   + y.e * (fabs(x.z.re) + fabs(x.z.im));
  double e = (1+4*EPS)*( ((5*HALFEPS)*A + (1+103*EPS)*B) / nrm);
  return AComplex( (xryr+xiyi)/nrm, (xiyr-xryi)/nrm, e);

@ If |x| is |XComplex|, then
$\reps{|sqrt(x)|} \supset \sqrt{\reps{|x|}}$.

@<Definition of |sqrt(x)| for |XComplex x|@>=
  double s = sqrt((fabs(x.re) + hypot(x.re, x.im)) * 0.5);
  double d = (x.im / s) * 0.5;
  double e = EPS*((1+4*EPS)*(1.25*s+1.75*fabs(d)));
  if (x.re > 0.0)
    return AComplex(s, d, e);
  else
    return AComplex(d, s, e);

@ If |x| is |XComplex|, then
$\reps{|absUB(x)|} \ge \abs{\reps{|x|}}$.
@<Definition of |absUB(x)| for |XComplex x|@>=
  return (1+2*EPS)*hypot(x.re, x.im);

@ If |x| is |XComplex|, then
$\reps{|absLB(x)|} \le \abs{\reps{|x|}}$.
@<Definition of |absLB(x)| for |XComplex x|@>=
  return (1-2*EPS)*hypot(x.re, x.im);

