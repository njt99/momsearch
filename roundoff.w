@*Computer arithmetic.
The arithmetic operations defined on a computer are of necessity inexact.
In this section, we discuss the relevant properties of computer 
arithmetic.
We also define the constants involved, and the procedures
needed to control the floating-point behavior.

The model we use for floating-point arithmetic is the IEEE-754
double-precision standard; it is used on all of the computers the computation
is run on.

It would be possible to regard floating-point numbers
as a subset of the rationals.
Instead, we regard them as separate objects, and use a map
$S : |double| \ra \Real$.

This program does not allow underflow.
We ask the system to signal an error if underflow occurs;
and the error bounds we use are only valid when there is no underflow.

In order to understand the error bounds, we need to introduce
some properties of floating-point arithmetic.
The IEEE standard implies (assuming there is no underflow)
that the error of the operations
|+|, |-|, |*|, |/| and |sqrt| is bounded by $|EPS|/2$ times
the absolute value of the result.  More specifically, there is
a finite set of numbers which are representable on the computer,
and the result of these operations is always the closest representable
number to the true solution.
The standard also implies that if $|n|$ is not too large and $x$ is
representable, then $1+n |EPS|$, $2^n x$ and $-x$ are representable
and that
$\reps{|(1+EPS)*x|} > \reps{x}$ and $\reps{|(1-EPS)*x|} < \reps{|x|}$
for $x \neq 0$.

Finally, according to the documentation provided for the |hypot| function,
there cannot be a representable number between
$\reps{|hypot(x)|}$ and $\sqrt{\reps{x}}$.

@<Definition of |EPS| and |HALFEPS|@>=
#define EPS DBL_EPSILON
#define HALFEPS (EPS/2)

@ 
It is unfortunate that there is no standard way to use the IEEE functions
for floating-point control.  We have to use a different
method for each kind of machine.
@<Declaration of floating-point functions@>=
#ifdef __GNUG__
inline double fabs(double x) { return x < 0 ? -x : x; }
extern "C" {
#endif /* C++ */
  void initialize_roundoff();
  int roundoff_ok();
#ifndef __sparc__
  extern double infinity();
#endif /* sparc */
#ifdef __GNUG__
}
#endif /* C++ */

@ @<Definition of floating-point functions@>=
#ifndef __sparc__
inline double infinity() { return 1.0/0.0; }
#endif

#ifdef sgi
#include <sys/fpu.h>

void initialize_roundoff() { 
  union fpc_csr csr;
  csr.fc_word = get_fpc_csr();
  csr.fc_struct.en_underflow = 1;
  set_fpc_csr(csr.fc_word);
}
#else 
#ifdef __sparc__
#include <floatingpoint.h>
void initialize_roundoff() {
  ieee_handler("set","underflow",SIGFPE_ABORT);
}
#else /* sparc */
void initialize_roundoff() {
}
#endif /* sparc */
#endif /* sgi */


#if defined(sgi) || defined(__sparc__)
int roundoff_ok() {
  return 1;
}
#else /* sgi || sparc */
#if defined(AIX)
int roundoff_ok() {
  return fp_underflow() == 0;
}
#else /* AIX */
int roundoff_ok() {
  return 0;
}
#endif /* AIX */
#endif /* sgi || sparc */

