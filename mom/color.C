// M = [1, m, 0, 1]
// N = [1, m n + q i, 0, 1]
// G = [i exp(-i theta/2) (g m + h (m n + q i)), i exp(i theta/2), i exp(-i theta/2), 0]

// 1 <= m  (horoball size)
// -1/2 <= f <= 1/2 (reduction modulo M)
// m^2 (1-f^2) <= q^2 (M isn't longer than N)
// 0 <= g <= 1/2 (reduction modulo M, flipping sign of M)
// 0 <= h <= 1/2 (reduction modulo N, flipping sign of N)
// q > 0 (complex conjugate symmetry)
// m q <= 4 (area of fundamental parallelogram)
// -1 < t <= 1
#include "GL2C.h"

GL2C constructM(double m, double q, double f, double g, double h, double t)
{
	return GL2C( 1, m, 0, 1);
}

GL2C constructN(double m, double q, double f, double g, double h, double t)
{
	return GL2C( 1, Complex(m*f, q), 0, 1);
}

GL2C constructG(double m, double q, double f, double g, double h, double t)
{
	double x = 2*t / (1 + t*t);
	double y = (1-t*t) / (1+t*t);

	Complex ieit(-y, x);
	Complex iemit(-y, -x);
	Complex trans(g*m + h*m*f, h*q);
	return GL2C(iemit*trans, ieit, iemit, 0);
}
	
void print(GL2C& m, const char* what) {
	printf("%10s%9g+%9gI\t%9g+%9gI\n%10s%9g+%9gI\t%9g+%9gI\n",
		what, m.a.real(), m.a.imag(), m.b.real(), m.b.imag(),
		"", m.c.real(), m.c.imag(), m.d.real(), m.d.imag());
}
int main(int argc, char** argv)
{
	GL2C Gminus = constructG(1, 0.9, 0.8, 0.7, 0.6, -1);
	GL2C Gplus = constructG(1, 0.9, 0.8, 0.7, 0.6, 1);
	GL2C Gzero = constructG(1, 0.9, 0.8, 0, 0, 0);
	GL2C Gwhitehead = constructG(sqrt(2), 2*sqrt(2), 0, 0, 0.5, 0);
	print(Gminus, "G-");
	print(Gplus, "G+");
	print(Gzero, "G0");
	print(Gwhitehead, "GWhitehead");
	return 0;
}
