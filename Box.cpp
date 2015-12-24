/*
 *  Box.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */

#include "Box.h"
#include <math.h>

// ULP functions
// TODO: CHECK FOR OVERLOW AND UNDERFLOW
double inc_d(double x) {
   return nextafter(x,x+1); 
}

double dec_d(double x) {
   return nextafter(x,x-1); 
}

double scale[6];
static bool scaleInitialized = false; 
Box::Box() {
	if (!scaleInitialized) {
		scaleInitialized = true;
		for (int i = 0; i < 6; ++i) {
			scale[i] = 8 * pow(2, -i / 6.0);
		}
	}
	for (int i = 0; i < 6; ++i) {
		centerDigits[i] = 0;
		sizeDigits[i] = 1;
	}
	pos = 0;
}

Box Box::child(int dir) const
{
	Box child(*this);
	child.sizeDigits[pos] *= 0.5;
	child.centerDigits[pos] += (2*dir-1)*child.sizeDigits[pos];
//	printf("digits[%d] = %f\n", pos, child.centerDigits[pos]);
	++child.pos;
	if (child.pos == 6)
		child.pos = 0;
	return child;
}

Params<Complex> Box::center() const
{
	Params<Complex> result;
	result.lattice = Complex(scale[3]*centerDigits[3], scale[0]*centerDigits[0]);
	result.loxodromicSqrt = Complex(scale[4]*centerDigits[4], scale[1]*centerDigits[1]);
	result.parabolic = Complex(scale[5]*centerDigits[5], scale[2]*centerDigits[2]);
	return result;
}

Params<Complex> Box::offset(const double* offset) const
{
	Params<Complex> result;
	result.lattice = Complex(
		scale[3]*(offset[3]*sizeDigits[3] + centerDigits[3]),
		scale[0]*(offset[0]*sizeDigits[0] + centerDigits[0])
	);
	result.loxodromicSqrt = Complex(
		scale[4]*(offset[4]*sizeDigits[4] + centerDigits[4]),
		scale[1]*(offset[1]*sizeDigits[1] + centerDigits[1])
	);
	result.parabolic = Complex(
		scale[5]*(offset[5]*sizeDigits[5] + centerDigits[5]),
		scale[2]*(offset[2]*sizeDigits[2] + centerDigits[2])
	);
	return result;
}

// Note: sizeDigits is always positive
Params<Complex> Box::nearest() const
{
	int TODO_ULP; // TODO Verify ULP and add OVERFLOW and UNDERFLOW
	double m[6];
    double temp;
	for (int i = 0; i < 6; ++i) {
		if (centerDigits[i] < 0) {
            temp = inc_d(centerDigits[i]+sizeDigits[i]);
            if (temp > 0 ) { // Check if we have overlapped 0
                m[i] = 0;
            } else { // We know scale is positive
		    	m[i] = inc_d(scale[i]*temp);
                if (m[i] > 0) { // If temp was 0
                    m[i] = 0;
                }
            }  
        } else {
            temp = dec_d(centerDigits[i]-sizeDigits[i]);
            if (temp < 0 ) { // Check if we have overlapped 0
                m[i] = 0;
            } else { // We know scale is positive
		    	m[i] = dec_d(scale[i]*temp);
                if (m[i] < 0) { // If temp was 0
                    m[i] = 0;
                }
            }  
        }
	}
	
	Params<Complex> result;
	result.lattice = Complex(m[3], m[0]);
	result.loxodromicSqrt = Complex(m[4], m[1]);
	result.parabolic = Complex(m[5], m[2]);
	return result;
}

// Note: sizeDigits is always positive
Params<Complex> Box::furthest() const
{
	int TODO_ULP;
	double m[6];
	for (int i = 0; i < 6; ++i) {
		if (centerDigits[i] < 0) {
		    m[i] = dec_d(scale[i]*dec_d(centerDigits[i]-sizeDigits[i]));
        } else {
		    m[i] = inc_d(scale[i]*inc_d(centerDigits[i]+sizeDigits[i]));
        }
	}
	
	Params<Complex> result;
	result.lattice = Complex(m[3], m[0]);
	result.loxodromicSqrt = Complex(m[4], m[1]);
	result.parabolic = Complex(m[5], m[2]);
	return result;
}

// Note: sizeDigits is always positive
Params<Complex> Box::minimum() const
{
	int TODO_ULP;
	double m[6];
	for (int i = 0; i < 6; ++i) {
        m[i] = dec_d(scale[i]*dec_d(centerDigits[i]-sizeDigits[i]));
	}
	
	Params<Complex> result;
	result.lattice = Complex(m[3], m[0]);
	result.loxodromicSqrt = Complex(m[4], m[1]);
	result.parabolic = Complex(m[5], m[2]);
	return result;
}

// Note: sizeDigits is always positive
Params<Complex> Box::maximum() const
{
	int TODO_ULP;
	double m[6];
	for (int i = 0; i < 6; ++i) {
        m[i] = inc_d(scale[i]*inc_d(centerDigits[i]+sizeDigits[i]));
	}
	
	Params<Complex> result;
	result.lattice = Complex(m[3], m[0]);
	result.loxodromicSqrt = Complex(m[4], m[1]);
	result.parabolic = Complex(m[5], m[2]);
	return result;
}

Params<AComplex1Jet> Box::cover() const
{
	Params<AComplex1Jet> result;
	result.lattice = AComplex1Jet(
		Complex(scale[3]*centerDigits[3], scale[0]*centerDigits[0]),
		Complex(scale[3]*sizeDigits[3], scale[0]*sizeDigits[0]),
		0.,
		0.
	);
	result.loxodromicSqrt = AComplex1Jet(
		Complex(scale[4]*centerDigits[4], scale[1]*centerDigits[1]),
		0.,
		Complex(scale[4]*sizeDigits[4], scale[1]*sizeDigits[1]),
		0.
	);
	result.parabolic = AComplex1Jet(
		Complex(scale[5]*centerDigits[5], scale[2]*centerDigits[2]),
		0.,
		0.,
		Complex(scale[5]*sizeDigits[5], scale[2]*sizeDigits[2])
	);
	return result;
}

void Box::volumeRange(double& low, double& high) const
{
	Params<Complex> min = nearest();
	Params<Complex> max = furthest();
	
	Complex minSl = min.loxodromicSqrt;
	Complex maxSl = max.loxodromicSqrt;
    // Note: we assume that lattice.imag() is positive in the box
	low = (minSl.real()*minSl.real() + minSl.imag()*minSl.imag()) * min.lattice.imag();
	high = (maxSl.real()*maxSl.real() + maxSl.imag()*maxSl.imag()) * max.lattice.imag();
}

double Box::size() const
{
    double box_size = 1;
	for (int i = 0; i < 6; ++i) {
		box_size *= scale[i]*sizeDigits[i];
    }
    return box_size;
}

NamedBox NamedBox::child(int dir) const
{
	NamedBox child(Box::child(dir));
	child.name = name;
	child.name.append(1, '0'+dir);
	child.qr = qr;
	return child;
}
