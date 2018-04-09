#ifndef __Box_h
#define __Box_h
#include "Params.h"
#include "ACJ.h"
#include "QuasiRelators.h"

struct Box {
	Box();
	Params<XComplex> center() const;
//	Params<XComplex> offset(const double* offset) const;
	Params<ACJ> cover() const;
	Params<XComplex> nearest() const; // returns closest to 0 or 0 if box overlaps
	Params<XComplex> minimum() const; // minimizes all values
	Params<XComplex> furthest() const; // furthest from 0
	Params<XComplex> maximum() const; // maximizes all values
	Box child(int dir) const;
private:
	double centerDigits[6];
	double sizeDigits[6];
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

#endif // __Box_h
