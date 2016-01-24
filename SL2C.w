@*Matrices of Complex numbers.
This section implements some operations on matrices of |XComplex|s.
It is used to approximate isometries of $H^3$.

An |SL2C| $g$ is a tuple |(a,b,c,d)| of |XComplex|s,
which represents the class $\reps{g}$ of functions
$h : \cube \ra \PSL_2(\Complex)$
such that $h_{0,0} \in \reps{a}$, $h_{0,1} \in \reps{b}$,
$h_{1,0} \in \reps{c}$, and $h_{1,1} \in \reps{d}$.
@<Definition of |SL2C|@>=
struct SL2C {
  SL2C() :a(1),b(0),c(0),d(1) { }
  SL2C(const XComplex& aa, const XComplex& bb, const XComplex& cc, const XComplex& dd)
     :a(aa),b(bb),c(cc),d(dd) { }
  XComplex a,b,c,d;
};

@ \proposition{M*J}
If |x| and |y| are |SL2C|, then we approximate the product with machine product and sum.
\endproposition
@<Definition of |x*y| for |SL2C x,y|@>=
  return SL2C(
    (x.a*y.a+x.b*y.c).z, (x.a*y.b+x.b*y.d).z, 
    (x.c*y.a+x.d*y.c).z, (x.c*y.b+x.d*y.d).z
  );

@ \proposition{inverse(M)}
If |x| is |SL2C|, then
\endproposition
$\reps{|inverse(x)|} = \reps{x}^{-1}$.
@<Definition of |inverse(x)| for |SL2C x|@>=
  return SL2C(x.d, -x.b, -x.c, x.a);
