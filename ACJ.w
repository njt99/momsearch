@*Approximate Complex $1$-Jets.
This section implements intervals of Complex $1$-Jets.
It is used to provide rigorous bounds
on the values a complex function takes on a region.

Let $\cube$ be the set of points $(z_0, z_1, z_2) \in \Complex^3$
such that $\abs{z_k} <= 1$ for $k = 0,1,2$.

An |ACJ| $j$ is a tuple $(f, f_0, f_1, f_2, e)$
with $f,f_0,f_1,f_2 \in \Cdouble$ and $e \in \Rdouble$,
which represents the class $\reps{|j|}$ of functions $g : \cube \ra \Complex$
such that such that
$$\abs{f(z_0,z_1,z_2) - (f + f_0 z_0 + f_1 z_1 + f_2 z_2)} < e$$
for all $(z_0,z_1,z_2) \in \cube$.  For convenience, we also define
$\reps{x} = \{|x|\}$ for $x \in \Complex$

The notation suggests that $f = h(0,0,0)$
and $f_k = \partial_k{h}(0,0,0)$.
This will usually be approximately true.

@<Definition of |ACJ|@>=
struct ACJ {
  ACJ(const XComplex& ff,   const XComplex& ff0=0,
               const XComplex& ff1=0, const XComplex& ff2=0, double err=0)
	       :f(ff),f0(ff0),f1(ff1),f2(ff2),e(err),
	       size( (1+2*EPS)*(absUB(ff0) + (absUB(ff1) + absUB(ff2))))
	       { }

  XComplex f;
  XComplex f0;
  XComplex f1;
  XComplex f2;
  double e;
  
  double size;
};

@ \proposition{-J}
If |x| is |ACJ|, then
$\reps{-x} = -\reps{x}$.
\endproposition
@<Definition of |-x| for |ACJ x|@>=
  return ACJ(-x.f, -x.f0, -x.f1, -x.f2, x.e);

@ \proposition{J+J}
If |x| and |y| are |ACJ|, then
$\reps{x+y} \supset \reps{x} + \reps{y}$.
\endproposition
@<Definition of |x+y| for |ACJ x,y|@>=
  AComplex r_f = x.f + y.f;
  AComplex r_f0 = x.f0 + y.f0;
  AComplex r_f1 = x.f1 + y.f1;
  AComplex r_f2 = x.f2 + y.f2;
  double r_error = (1+3*EPS)*(
      (x.e + y.e)
    + ((r_f.e + r_f0.e) + (r_f1.e + r_f2.e))
  );
  return ACJ(r_f.z, r_f0.z, r_f1.z, r_f2.z, r_error);

@ \proposition{J-J}
If |x| and |y| are |ACJ|, then
$\reps{x-y} \supset \reps{x} - \reps{y}$.
\endproposition
@<Definition of |x-y| for |ACJ x,y|@>=
  AComplex r_f = x.f - y.f;
  AComplex r_f0 = x.f0 - y.f0;
  AComplex r_f1 = x.f1 - y.f1;
  AComplex r_f2 = x.f2 - y.f2;
  double r_error = (1+3*EPS)*(
      (x.e + y.e)
    + ((r_f.e + r_f0.e) + (r_f1.e + r_f2.e))
  );
  return ACJ(r_f.z, r_f0.z, r_f1.z, r_f2.z, r_error);

@ \proposition{J+d}
If |x| is |ACJ| and |y| is |double|, then
$\reps{x+y} \supset \reps{x} + \reps{y}$.
\endproposition
@<Definition of |x+y| for |ACJ x| and |double y|@>=
  AComplex r_f = x.f+y;
  return ACJ( r_f.z, x.f0,x.f1,x.f2,
    (1+EPS)*(x.e + r_f.e));

@ \proposition{J-d}
If |x| is |ACJ| and |y| is |double|, then
$\reps{x-y} \supset \reps{x} - \reps{y}$.
\endproposition
@<Definition of |x-y| for |ACJ x| and |double y|@>=
  AComplex r_f = x.f - y;
  return ACJ( r_f.z, x.f0,x.f1,x.f2,
    (1+EPS)*(x.e + r_f.e));

@ \proposition{J*J}
If |x| and |y| are |ACJ|, then
$\reps{x*y} \supset \reps{x} \reps{y}$.
\endproposition
@<Definition of |x*y| for |ACJ x,y|@>=
  double xdist = size(x);
  double ydist = size(y);
  double ax = absUB(x.f), ay = absUB(y.f);
  AComplex r_f = x.f*y.f;
  AComplex r_f0 = x.f*y.f0 + x.f0*y.f;
  AComplex r_f1 = x.f*y.f1 + x.f1*y.f;
  AComplex r_f2 = x.f*y.f2 + x.f2*y.f;
  double A = (xdist+x.e)*(ydist+y.e);
  double B = ax*y.e + ay*x.e;
  double C = (r_f.e + r_f0.e) + (r_f1.e + r_f2.e);
  double r_error = (1+3*EPS) * ( A+(B+C));
  return ACJ(r_f.z, r_f0.z, r_f1.z, r_f2.z, r_error);

@ \proposition{J*d}
If |x| is |ACJ| and |y| is |double|, then
$\reps{x*y} \supset \reps{x} \reps{y}$.
\endproposition
@<Definition of |x*y| for |ACJ x| and |double y|@>=
  AComplex r_f = x.f*y;
  AComplex r_f0 = x.f0*y;
  AComplex r_f1 = x.f1*y;
  AComplex r_f2 = x.f2*y;
  double r_error = (1+3*EPS)*(
      (x.e*fabs(y))
    + ((r_f.e + r_f0.e) + (r_f1.e + r_f2.e))
  );
  return ACJ(r_f.z, r_f0.z, r_f1.z, r_f2.z, r_error);

@ \proposition{J/J}
If |x| and |y| are |ACJ|, then
$\reps{x/y} \supset \reps{x} / \reps{y}$.
\endproposition
@<Definition of |x/y| for |ACJ x,y|@>=
  double xdist = size(x);
  double ydist = size(y);
  double ax = absUB(x.f), ay = absLB(y.f);
  double D = ay - (1+EPS)*(y.e+ydist);
  if (!(D > 0)) return ACJ(0,0,0,0,infinity());
  AComplex den = (y.f*y.f);
  AComplex r_f = x.f/y.f;
  AComplex r_f0 = (x.f0*y.f - x.f*y.f0)/den;
  AComplex r_f1 = (x.f1*y.f - x.f*y.f1)/den;
  AComplex r_f2 = (x.f2*y.f - x.f*y.f2)/den;
  double A = (ax+(xdist+x.e))/D;
  double B = (ax/ay+xdist/ay) + (ydist*ax)/(ay*ay);
  double C = (r_f.e + r_f0.e) + (r_f1.e + r_f2.e);
  double r_error = (1+3*EPS) * (((1+3*EPS)*A -(1-3*EPS)*B) + C);
  return ACJ(r_f.z, r_f0.z, r_f1.z, r_f2.z, r_error);

@ \proposition{d/J}
If |x| is |double| and |y| is |ACJ|, then
$\reps{x/y} \supset \reps{x} / \reps{y}$.
\endproposition
@<Definition of |x/y| for |double x| and |ACJ y|@>=
  double ydist = size(y);
  double ax = fabs(x), ay = absLB(y.f);
  double D = ay - (1+EPS)*(y.e+ydist);
  if (!(D > 0)) return ACJ(0,0,0,0,infinity());
  AComplex den = (y.f*y.f);
  AComplex r_f = x/y.f;
  AComplex r_f0 = (-x*y.f0)/den;
  AComplex r_f1 = (-x*y.f1)/den;
  AComplex r_f2 = (-x*y.f2)/den;
  double B = ax/ay + (ydist*ax)/(ay*ay);
  double C = (r_f.e + r_f0.e) + (r_f1.e + r_f2.e);
  double r_error = (1+3*EPS) * ( ((1+2*EPS)*(ax/D) -(1-3*EPS)*B) + C);
  return ACJ(r_f.z, r_f0.z, r_f1.z, r_f2.z, r_error);

@ \proposition{J/d}
If |x| is |ACJ| and |y| is |double|, then
$\reps{|x/y|} \supset \reps{x} / \reps{y}$.
\endproposition
@<Definition of |x/y| for |ACJ x| and |double y|@>=
  AComplex r_f = x.f/y;
  AComplex r_f0 = x.f0/y;
  AComplex r_f1 = x.f1/y;
  AComplex r_f2 = x.f2/y;
  double r_error = (1+3*EPS)*(
      (x.e/fabs(y))
    + ((r_f.e + r_f0.e) + (r_f1.e + r_f2.e))
  );
  return ACJ(r_f.z, r_f0.z, r_f1.z, r_f2.z, r_error);

@ \proposition{|sqrt(J)|}
If |x| is |ACJ|, then
$\reps{|sqrt(x)|} \supset \sqrt{\reps{x}}$.
\endproposition
@<Definition of |sqrt(x)| for |ACJ x|@>=
  double xdist = size(x);
  double ax = absUB(x.f);
  double D = ax - (1+EPS)*(xdist+x.e);
  if (!(D > 0)) {
    return ACJ(0, 0, 0, 0, (1+2*EPS)*sqrt(ax + (xdist + x.e)));
  } else {
    AComplex r_f = sqrt(x.f);
    AComplex t = r_f+r_f;
    AComplex r_f0 = AComplex(x.f0.re,x.f0.im,0) / t;
    AComplex r_f1 = AComplex(x.f1.re,x.f1.im,0) / t;
    AComplex r_f2 = AComplex(x.f2.re,x.f2.im,0) / t;
    double r_error = (1+3*EPS) * (
        ((1+EPS)*sqrt(ax)
        -(1-3*EPS)*(xdist/(2*sqrt(ax)) + sqrt(D)))
      + ((r_f.e + r_f0.e) + (r_f1.e + r_f2.e))
    );
    return ACJ(r_f.z, r_f0.z, r_f1.z, r_f2.z, r_error);
  }

@ \proposition{|absUB(J)|}
If |x| is |ACJ|, then
$\reps{|absUB(x)|} >= \reps{x}(z)$ for all $z \in \cube$.
\endproposition
@<Definition of |absUB(x)| for |ACJ x|@>=
  return (1+2*EPS)*(absUB(x.f) + (size(x) + x.e));

@ \proposition{|absLB(J)|}
If |x| is |ACJ|, then
$\reps{|absLB(x)|} <= \reps{x}(z)$ for all $z \in \cube$.
\endproposition
@<Definition of |absLB(x)| for |ACJ x|@>=
  double v = (1-EPS)*(absLB(x.f) - (1+EPS)*(size(x) + x.e));
  return (v > 0) ? v : 0;

@ \proposition{|size(J)|}
If |(j.f, j.f0, j.f1, j.f2, j.error)| is |ACJ|, then
$\reps{|size(j)|} \ge \abs{\reps{|j.f0|}} + \abs{\reps{|j.f1|}} + \abs{\reps{|j.f2|}}$.
\endproposition
@<Definition of |size(x)| for |ACJ x|@>=
  return x.size;
