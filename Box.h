/*
 *  Box.h
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */
#ifndef __Box_h
#define __Box_h
#include "Params.h"
#include "AComplex1Jet.h"
#include "QuasiRelators.h"

struct Box {
	Box();
	Params<Complex> center() const;
	Params<Complex> offset(const double* offset) const;
	Params<AComplex1Jet> cover() const;
	Params<Complex> minimum() const; // closest to 0
	Params<Complex> maximum() const; // furthest from 0
	void volumeRange(double& low, double& high) const;
    double size() const;
	Box child(int dir) const;
private:
	double centerDigits[6];
	double sizeDigits[6];
	int pos;
};

struct NamedBox : public Box {
	NamedBox() {}
	NamedBox(Box box) :Box(box) {}
	
	std::string name;
	QuasiRelators qr;
	NamedBox child(int dir) const;
};

#endif // __Box_h
