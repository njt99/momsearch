#ifndef __Box_h
#define __Box_h
#include "Params.h"
#include "ACJ.h"
#include "QuasiRelators.h"

struct Box {
	Box();
	Params<XComplex> center() const;
	Box child(int dir) const;
	Params<ACJ> cover() const;
	Params<XComplex> nearer() const; // returns all values closer to 0 than in box or 0 if box overlaps
	Params<XComplex> further() const; // returns all values futher from 0 that in the box
	Params<XComplex> greater() const; // returns all values greater than in the box
//	Params<XComplex> offset(const double* offset) const;
private:
	double center_digits[6];
	double size_digits[6];
    double box_center[6];
    double box_size[6];
	int pos;
    void compute_center_and_size();
};

struct NamedBox : public Box {
	NamedBox() {}
	NamedBox(Box box) :Box(box) {}
	
	std::string name;
	QuasiRelators qr;
	NamedBox child(int dir) const;
};

inline const double areaLB(const Params<XComplex>&nearer)
{
    // Area is |lox_sqrt|^2*|Im(lattice)|.
    XComplex lox_sqrt = nearer.loxodromic_sqrt;
    double lat_im     = nearer.lattice.im;
    // Apply Lemma 7 of GMT.
    double lox_re = (1-EPS)*(lox_sqrt.re*lox_sqrt.re);
    double lox_im = (1-EPS)*(lox_sqrt.im*lox_sqrt.im);
    double lox_norm = (1-EPS)*(lox_re + lox_im);
    return (1-EPS)*(lox_norm*lat_im);
}

#endif // __Box_h
