@*Matrices of Approximate Complex $1$-Jets.
This section implements some operations on matrices of |ACJ|s.
It is used to model intervals of isometries of $H^3$.

An |SL2ACJ| $g$ is a tuple |(a,b,c,d)| of |ACJ|s,
which represents the class $\reps{g}$ of functions
$h : \cube \ra \PSL_2(\Complex)$
such that $h_{0,0} \in \reps{a}$, $h_{0,1} \in \reps{b}$,
$h_{1,0} \in \reps{c}$, and $h_{1,1} \in \reps{d}$.
@<Definition of |SL2ACJ|@>=
struct SL2ACJ {
  SL2ACJ() :a(1),b(0),c(0),d(1) { }
  SL2ACJ(const ACJ& aa, const ACJ& bb, const ACJ& cc, const ACJ& dd)
     :a(aa),b(bb),c(cc),d(dd) { }
  ACJ a,b,c,d;
};

@ \proposition{M*J}
If |x| and |y| are |SL2ACJ|, then
$\reps{|x * y|} \supset \reps{x} \reps{y}$.
\endproposition
@<Definition of |x*y| for |SL2ACJ x,y|@>=
  return SL2ACJ(
    x.a*y.a+x.b*y.c, x.a*y.b+x.b*y.d, 
    x.c*y.a+x.d*y.c, x.c*y.b+x.d*y.d
  );

@ \proposition{M+J}
If |x| and |y| are |SL2ACJ|, then
$\reps{|x + y|} \supset \reps{x} + \reps{y}$.
\endproposition
@<Definition of |x+y| for |SL2ACJ x,y|@>=
  return SL2ACJ(
    x.a+y.a, x.b+y.b, 
    x.c+y.c, x.d+y.d
  );

@ \proposition{M-J}
If |x| and |y| are |SL2ACJ|, then
$\reps{|x - y|} \supset \reps{x} - \reps{y}$.
\endproposition
@<Definition of |x-y| for |SL2ACJ x,y|@>=
  return SL2ACJ(
    x.a-y.a, x.b-y.b, 
    x.c-y.c, x.d-y.d
  );

@ \proposition{inverse(M)}
If |x| is |SL2ACJ|, then
\endproposition
$\reps{|inverse(x)|} = \reps{x}^{-1}$.
@<Definition of |inverse(x)| for |SL2ACJ x|@>=
  return SL2ACJ(x.d, -x.b, -x.c, x.a);

@ \proposition{notIdentity(M)}
If |x| is |SL2ACJ|, then
|notIdentity(x)| returns $1$ implies $\pm I \notin \reps{x}$
\endproposition
@<Definition of |notIdentity(x)| for |SL2ACJ x|@>=
  return absLB(x.b) > 0
      || absLB(x.c) > 0
      || (absLB(x.a-1) > 0 && absLB(x.a+1) > 0)
      || (absLB(x.d-1) > 0 && absLB(x.d+1) > 0);

@ \proposition{notZero(M)}
If |x| is |SL2ACJ|, then
|notZero(x)| returns $1$ implies $((0,0),(0,0)) \notin \reps{x}$
\endproposition
@<Definition of |notZero(x)| for |SL2ACJ x|@>=
  return absLB(x.a) > 0
      || absLB(x.b) > 0
      || absLB(x.c) > 0
      || absLB(x.d) > 0;
