#include "Box.h"

double scale[6];
static bool scale_initialized = false; 
Box::Box() {
	if (!scale_initialized) {
		scale_initialized = true;
		for (int i = 0; i < 6; ++i) {
			scale[i] = pow(2, -i / 6.0);
		}
	}
	for (int i = 0; i < 6; ++i) {
		centerDigits[i] = 0;
		size_digits[i] = 8;
	}
	pos = 0;
    compute_center_and_size();
}

Box Box::child(int dir) const
{
	Box child(*this);
	child.size_digits[pos] *= 0.5;
	child.centerDigits[pos] += (2*dir-1)*child.size_digits[pos];
	++child.pos;
	if (child.pos == 6)
		child.pos = 0;
    child.compute_center_and_size();
	return child;
}

void Box::compute_center_and_size()
{
	for (int i = 0; i < 6; ++i) {
        // GMT paper page 419 of Annals
        // box_size guarantees that :
        // box_center - box_size <= true_center - true_size
        // box_center + box_size >= true_center + true_size
        // where box operations are floating point. 
        box_center[i] = scale[i]*centerDigits[i];
        box_size[i]= (1+2*EPS)*(size_digits[i]*scale[i]+HALFEPS*fabs(centerDigits[i]));
    }

}

Params<XComplex> Box::center() const
{
    Params<XComplex> result;
    result.lattice = XComplex(box_center[3], box_center[0]);
    result.loxodromic_sqrt = XComplex(box_center[4], box_center[1]);
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
	result.loxodromic_sqrt = ACJ(
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

Params<XComplex> Box::nearest() const
{
	double m[6];
    double temp;
	for (int i = 0; i < 6; ++i) {
		if (center_digits[i] < 0) {
            // GMT paper page 419 of Annals
            // temp is guaranteed to be >= than true_center + true_size
            temp = box_center[i]+box_size[i];
            if (temp > 0 ) { // Check if we have overlapped 0
                m[i] = 0;
            } else { // We know scale is positive
		    	m[i] = temp;
            }  
        } else {
            // GMT paper page 419 of Annals
            // temp is guaranteed to be <= than true_center - true_size
            temp = box_center[i]-box_size[i];
            if (temp < 0 ) { // Check if we have overlapped 0
                m[i] = 0;
            } else { // We know scale is positive
		    	m[i] = temp;
            }  
        }
	}
	
	Params<XComplex> result;
	result.lattice = XComplex(m[3], m[0]);
	result.loxodromic_sqrt = XComplex(m[4], m[1]);
	result.parabolic = XComplex(m[5], m[2]);
	return result;
}

Params<XComplex> Box::furthest() const
{
	double m[6];
	for (int i = 0; i < 6; ++i) {
		if (center_digits[i] < 0) {
            // GMT paper page 419 of Annals
            // guaranteed to be <= than true_center - true_size
		    m[i] = box_center[i]-box_size[i];
        } else {
            // guaranteed to be >= than true_center + true_size
		    m[i] = box_center[i]+box_size[i];
        }
	}
	
	Params<XComplex> result;
	result.lattice = XComplex(m[3], m[0]);
	result.loxodromic_sqrt = XComplex(m[4], m[1]);
	result.parabolic = XComplex(m[5], m[2]);
	return result;
}

// Note: size_digits is always positive
Params<XComplex> Box::maximum() const
{
	double m[6];
	for (int i = 0; i < 6; ++i) {
        // GMT paper page 419 of Annals
        // guaranteed to be >= than true_center + true_size
        m[i] = box_center[i]+box_size[i];
	}
	
	Params<XComplex> result;
	result.lattice = XComplex(m[3], m[0]);
	result.loxodromic_sqrt = XComplex(m[4], m[1]);
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

//Params<XComplex> Box::offset(const double* offset) const
//{
//	Params<XComplex> result;
//	result.lattice = XComplex(
//		scale[3]*(offset[3]*size_digits[3] + centerDigits[3]),
//		scale[0]*(offset[0]*size_digits[0] + centerDigits[0])
//	);
//	result.loxodromic_sqrt = XComplex(
//		scale[4]*(offset[4]*size_digits[4] + centerDigits[4]),
//		scale[1]*(offset[1]*size_digits[1] + centerDigits[1])
//	);
//	result.parabolic = XComplex(
//		scale[5]*(offset[5]*size_digits[5] + centerDigits[5]),
//		scale[2]*(offset[2]*size_digits[2] + centerDigits[2])
//	);
//	return result;
//}
