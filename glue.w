@*Technical details.
This section contains the \CEE/++ ``glue'' which tells
the computer how to use the definitions of the operators
on |XComplex|, |AComplex|, |ACJ| and |SL2ACJ|.

@(roundoff.h@>=
#ifndef _roundoff_h_
#define _roundoff_h_
#include <float.h>
#include <math.h>
@<Definition of |EPS| and |HALFEPS|@>@;
@<Declaration of floating-point functions@>@;
#endif

@ @(roundoff.c@>=
#include "roundoff.h"
@<Definition of floating-point functions@>@;

@ @(Complex.h@>=
#ifndef _Complex_h_
#define _Complex_h_
#include <assert.h>
#include "roundoff.h"
@<Definition of |XComplex|@>@;
@<Definition of |AComplex|@>@;
inline const XComplex operator - (const XComplex& x);
inline const AComplex operator + (const AComplex& x, const AComplex& y);
inline const AComplex operator + (const XComplex& x, const XComplex& y);
inline const AComplex operator + (const XComplex& x, double y);
inline const AComplex operator - (const AComplex& x, const AComplex& y);
inline const AComplex operator - (const XComplex& x, const XComplex& y);
inline const AComplex operator - (const XComplex& x, double y);
inline const AComplex operator * (const XComplex& x, const XComplex& y);
inline const AComplex operator * (const XComplex& x, double y);
inline const AComplex operator / (const XComplex& x, double y);
inline const double absLB(const XComplex& x);
inline const double absUB (const XComplex& x);
AComplex operator / (const AComplex& x, const AComplex& y);
AComplex operator / (const XComplex& x, const XComplex& y);
AComplex operator / (double x, const XComplex& y);
AComplex sqrt (const XComplex& x);
#include "Complex.inline"
#endif

@ @(Complex.inline@>=
inline const XComplex operator - (const XComplex& x)
{@<Definition of |-x| for |XComplex x|@>@;}
inline const AComplex operator + (const XComplex& x, const XComplex& y)
{@<Definition of |x+y| for |XComplex x,y|@>@;}
inline const AComplex operator + (const XComplex& x, double y)
{@<Definition of |x+y| for |XComplex x| and |double y|@>@;}
inline const AComplex operator + (const AComplex& x, const AComplex& y)
{@<Definition of |x+y| for |AComplex x,y|@>@;}
inline const AComplex operator - (const XComplex& x, const XComplex& y)
{@<Definition of |x-y| for |XComplex x,y|@>@;}
inline const AComplex operator - (const XComplex& x, double y)
{@<Definition of |x-y| for |XComplex x| and |double y|@>@;}
inline const AComplex operator - (const AComplex& x, const AComplex& y)
{@<Definition of |x-y| for |AComplex x,y|@>@;}
inline const AComplex operator * (const XComplex& x, const XComplex& y)
{@<Definition of |x*y| for |XComplex x,y|@>@;}
inline const AComplex operator * (const XComplex& x, double y)
{@<Definition of |x*y| for |XComplex x| and |double y|@>@;}
inline const AComplex operator / (const XComplex& x, double y)
{@<Definition of |x/y| for |XComplex x| and |double y|@>@;}
inline const double absLB(const XComplex& x)
{@<Definition of |absLB(x)| for |XComplex x|@>@;}
inline const double absUB (const XComplex& x)
{@<Definition of |absUB(x)| for |XComplex x| @>@;}

@ @(Complex.C@>=
#include "Complex.h"
AComplex operator / (const AComplex& x, const AComplex& y)
{@<Definition of |x/y| for |AComplex x,y|@>@;}

AComplex operator / (const XComplex& x, const XComplex& y)
{@<Definition of |x/y| for |XComplex x,y|@>@;}

AComplex operator / (double x, const XComplex& y)
{@<Definition of |x/y| for |double x| and |XComplex y|@>@;}

AComplex sqrt (const XComplex& x)
{@<Definition of |sqrt(x)| for |XComplex x|@>@;}

@ @(ACJ.h@>=
#ifndef _ACJ_h_
#define _ACJ_h_
#include "Complex.h"
#include <assert.h>
#include <stdio.h>
#include "roundoff.h"
@<Definition of |ACJ|@>@;
inline const ACJ    operator - (const ACJ& x);
inline const ACJ    operator + (const ACJ& x, const ACJ& y);
inline const ACJ    operator - (const ACJ& x, const ACJ& y);
inline const ACJ    operator + (const ACJ& x, double y);
inline const ACJ    operator - (const ACJ& x, const ACJ& y);
inline const ACJ    operator * (const ACJ& x, double y);
inline const ACJ    operator / (const ACJ& x, double y);
inline const double absUB      (const ACJ& x);
inline const double absLB      (const ACJ& x);
inline const double size       (const ACJ& x);
const        ACJ    operator * (const ACJ& x, const ACJ& y);
const        ACJ    operator / (const ACJ& x, const ACJ& y);
const        ACJ    operator / (double x, const ACJ& y);
const        ACJ    sqrt       (const ACJ& x);
#include "ACJ.inline"
#endif

@ @(ACJ.inline@>=
inline const ACJ    operator - (const ACJ& x)
{@<Definition of |-x| for |ACJ x|@>@;}
inline const ACJ    operator + (const ACJ& x, const ACJ& y)
{@<Definition of |x+y| for |ACJ x,y|@>@;}
inline const ACJ    operator - (const ACJ& x, const ACJ& y)
{@<Definition of |x-y| for |ACJ x,y|@>@;}
inline const ACJ    operator + (const ACJ& x, double y)
{@<Definition of |x+y| for |ACJ x| and |double y|@>@;}
inline const ACJ    operator - (const ACJ& x, double y)
{@<Definition of |x-y| for |ACJ x| and |double y|@>@;}
inline const ACJ    operator * (const ACJ& x, double y)
{@<Definition of |x*y| for |ACJ x| and |double y|@>@;}
inline const ACJ    operator / (const ACJ& x, double y)
{@<Definition of |x/y| for |ACJ x| and |double y|@>@;}
inline const double absUB      (const ACJ& x)
{@<Definition of |absUB(x)| for |ACJ x|@>@;}
inline const double absLB      (const ACJ& x)
{@<Definition of |absLB(x)| for |ACJ x|@>@;}
inline const double size       (const ACJ& x)
{@<Definition of |size(x)| for |ACJ x|@>@;}

@ @(ACJ.C@>=
#include "ACJ.h"
const        ACJ    operator * (const ACJ& x, const ACJ& y)
{@<Definition of |x*y| for |ACJ x,y|@>@;}
const        ACJ    operator / (const ACJ& x, const ACJ& y)
{@<Definition of |x/y| for |ACJ x,y|@>@;}
const        ACJ    operator / (double x, const ACJ& y)
{@<Definition of |x/y| for |double x| and |ACJ y|@>@;}
const        ACJ    sqrt       (const ACJ& x)
{@<Definition of |sqrt(x)| for |ACJ x|@>@;}

@ @(SL2ACJ.h@>=
#ifndef _SL2ACJ_h_
#define _SL2ACJ_h_
#include "ACJ.h"
@<Definition of |SL2ACJ|@>@;
const SL2ACJ operator * (const SL2ACJ& x, const SL2ACJ& y);
const SL2ACJ inverse    (const SL2ACJ& x);
const int    notIdentity(const SL2ACJ& x);
#endif

@ @(SL2ACJ.C@>=
#include "SL2ACJ.h"
const SL2ACJ operator * (const SL2ACJ& x, const SL2ACJ& y)
{@<Definition of |x*y| for |SL2ACJ x,y|@>@;}
const SL2ACJ inverse    (const SL2ACJ& x)
{@<Definition of |inverse(x)| for |SL2ACJ x|@>@;}
const int    notIdentity(const SL2ACJ& x)
{@<Definition of |notIdentity(x)| for |SL2ACJ x|@>@;}
