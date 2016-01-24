/*
 *  Box.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */

#include "Box.h"

double scale[6];
static bool scaleInitialized = false; 
Box::Box() {
	if (!scaleInitialized) {
		scaleInitialized = true;
		for (int i = 0; i < 6; ++i) {
			scale[i] = pow(2, -i / 6.0);
		}
	}
	for (int i = 0; i < 6; ++i) {
		centerDigits[i] = 0;
		sizeDigits[i] = 8;
	}
	pos = 0;
    compute_center_and_size();
}

Box Box::child(int dir) const
{
	Box child(*this);
	child.sizeDigits[pos] *= 0.5;
	child.centerDigits[pos] += (2*dir-1)*child.sizeDigits[pos];
	++child.pos;
	if (child.pos == 6)
		child.pos = 0;
    child.compute_center_and_size();
	return child;
}

void Box::compute_center_and_size()
{
	for (int i = 0; i < 6; ++i) {
        // Reference the Annals paper for these bounds
        box_center[i] = scale[i]*centerDigits[i];
        box_size[i]= (1+2*EPS)*(sizeDigits[i]*scale[i]+HALFEPS*fabs(centerDigits[i]));
    }

}

Params<XComplex> Box::center() const
{
    Params<XComplex> result;
    result.lattice = XComplex(box_center[3], box_center[0]);
    result.loxodromicSqrt = XComplex(box_center[4], box_center[1]);
    result.parabolic = XComplex(box_center[5], box_center[2]);
	return result;
}

Params<ACJ> Box::cover() const
{
	Params<ACJ> result;
	result.lattice = ACJ(
		XComplex(box_center[3], box_center[0]),
		XComplex(box_size[3], box_size[0]),
		0.,
		0.
	);
	result.loxodromicSqrt = ACJ(
		XComplex(box_center[4], box_center[1]),
		0.,
		XComplex(box_size[4], box_size[1]),
		0.
	);
	result.parabolic = ACJ(
		XComplex(box_center[5], box_center[2]),
		0.,
		0.,
		XComplex(box_size[5], box_size[2])
	);
	return result;
}

//
//double Box::size() const
//{
//    double box_size = 1;
//	for (int i = 0; i < 6; ++i) {
//		box_size *= scale[i]*sizeDigits[i];
//    }
//    return box_size;
//}

//Params<XComplex> Box::offset(const double* offset) const
//{
//	Params<XComplex> result;
//	result.lattice = XComplex(
//		scale[3]*(offset[3]*sizeDigits[3] + centerDigits[3]),
//		scale[0]*(offset[0]*sizeDigits[0] + centerDigits[0])
//	);
//	result.loxodromicSqrt = XComplex(
//		scale[4]*(offset[4]*sizeDigits[4] + centerDigits[4]),
//		scale[1]*(offset[1]*sizeDigits[1] + centerDigits[1])
//	);
//	result.parabolic = XComplex(
//		scale[5]*(offset[5]*sizeDigits[5] + centerDigits[5]),
//		scale[2]*(offset[2]*sizeDigits[2] + centerDigits[2])
//	);
//	return result;
//}

Params<XComplex> Box::nearest() const
{
	double m[6];
    double temp;
	for (int i = 0; i < 6; ++i) {
		if (centerDigits[i] < 0) {
            temp = inc_d(box_center[i]+box_size[i]);
            if (temp > 0 ) { // Check if we have overlapped 0
                m[i] = 0;
            } else { // We know scale is positive
		    	m[i] = temp;
            }  
        } else {
            temp = dec_d(box_center[i]-box_size[i]);
            if (temp < 0 ) { // Check if we have overlapped 0
                m[i] = 0;
            } else { // We know scale is positive
		    	m[i] = temp;
            }  
        }
	}
	
	Params<XComplex> result;
	result.lattice = XComplex(m[3], m[0]);
	result.loxodromicSqrt = XComplex(m[4], m[1]);
	result.parabolic = XComplex(m[5], m[2]);
	return result;
}

Params<XComplex> Box::furthest() const
{
	double m[6];
	for (int i = 0; i < 6; ++i) {
		if (centerDigits[i] < 0) {
		    m[i] = dec_d(box_center[i]-box_size[i]);
        } else {
		    m[i] = inc_d(box_center[i]+box_size[i]);
        }
	}
	
	Params<XComplex> result;
	result.lattice = XComplex(m[3], m[0]);
	result.loxodromicSqrt = XComplex(m[4], m[1]);
	result.parabolic = XComplex(m[5], m[2]);
	return result;
}

Params<XComplex> Box::minimum() const
{
	double m[6];
	for (int i = 0; i < 6; ++i) {
        m[i] = dec_d(box_center[i]-box_size[i]);
	}
	
	Params<XComplex> result;
	result.lattice = XComplex(m[3], m[0]);
	result.loxodromicSqrt = XComplex(m[4], m[1]);
	result.parabolic = XComplex(m[5], m[2]);
	return result;
}

// Note: sizeDigits is always positive
Params<XComplex> Box::maximum() const
{
	int TODO_ULP;
	double m[6];
	for (int i = 0; i < 6; ++i) {
        m[i] = inc_d(box_center[i]+box_size[i]);
	}
	
	Params<XComplex> result;
	result.lattice = XComplex(m[3], m[0]);
	result.loxodromicSqrt = XComplex(m[4], m[1]);
	result.parabolic = XComplex(m[5], m[2]);
	return result;
}

NamedBox NamedBox::child(int dir) const
{
	NamedBox child(Box::child(dir));
	child.name = name;
	child.name.append(1, '0'+dir);
	child.qr = qr;
	return child;
}

//void Box::volumeRange(double& low, double& high) const
//{
//	Params<XComplex> min = nearest();
//	Params<XComplex> max = furthest();
//	
//	XComplex minSl = min.loxodromicSqrt;
//	XComplex maxSl = max.loxodromicSqrt;
//    // Note: we assume that lattice.imag() is positive in the box
//	low = (minSl.real()*minSl.real() + minSl.imag()*minSl.imag()) * min.lattice.imag();
//	high = (maxSl.real()*maxSl.real() + maxSl.imag()*maxSl.imag()) * max.lattice.imag();
//}
//
