/*
 * Slopes.h
 *
 * Defines interface for slope computations in search.
 * Currently focused on slope distance
 *
 */

#include "Slopes.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <numeric>

using namespace std;

const int slope_dist(slope s1, slope s2) {
    int dist = s1.first * s2.second - s1.second * s2.first;
    if (dist < 0) { return -dist; }
    else { return dist; }
}

const slope short_slope_dist_and_count(Box box) {
    slope error(-1,-1);
    vector< slope > short_slopes;
    Params<XComplex> nearer = box.nearer();
    // Params<XComplex> further = box.further();
    // Params<XComplex> greater = box.greater();
    Params<XComplex> center = box.center();
    Params<ACJ> cover = box.cover();
    ACJ S = cover.loxodromic_sqrt;
    ACJ L = cover.lattice;
    ACJ SL = S*L;
    fprintf(stderr, "Box %s has min area: %f center lat: %f + I %f lox: %f + I %f par: %f + I %f\n", box.name.c_str(), areaLB(nearer), center.lattice.re, center.lattice.im, center.loxodromic_sqrt.re, center.loxodromic_sqrt.im, center.parabolic.re, center.parabolic.im);
    if (nearer.lattice.im <= 0) {
        fprintf(stderr, "Error: lattice with zero imaginary part in box %s\n", box.name.c_str());
        return error;
    }
    if (fabs(nearer.lattice.re) > 0.5) {
        fprintf(stderr, "Error: abs(lattice real part) > 1/2 in box %s\n", box.name.c_str());
        return error;
    }
    double b = 0;
    fprintf(stderr, "absUB(S*6) is %f\n", absUB(S * 6)); 
    while (absLB((S*nearer.lattice.im)*b) <= 6) {
        double a = 0;  
        fprintf(stderr, "b is %d\n", (int) b);
        while ((1-EPS)*(absLB((S*a)*(S*a)) + absLB((SL*b)*(SL*b))) <= (1+EPS)*(36 + absUB((S*a)*(S*b)))) {
            fprintf(stderr, "a is %d and b is %d\n", (int) a, (int) b);
            for (int sgn = -1; sgn < 2; sgn += 2) {
                ACJ g = ((L * b) + (sgn * a))*S;
                // avoid long slopes, non-primatives, and overconting (-1,0) and (1,0) or (0,1)
                if ((a == 0 && b == 0) || (sgn == -1 && (a == 0 || b == 0)) ||
                    gcd((int) a, (int) b) > 1 || absLB(g) > 6) {
                    continue;
                }
                slope p((int) sgn*a, (int) b);
                short_slopes.push_back(p);
            }
            a += 1;
        }
        b += 1;
        fprintf(stderr, "absLB(b * nearer.lattice.im) is %f and  absUB(S*6) is %f\n", absLB(b * nearer.lattice.im), absUB(S * 6)); 
    }
    int num_slopes = short_slopes.size();
    int max_dist = 0;
    for ( auto it1 = short_slopes.begin(); it1 != short_slopes.end(); ++it1) {
        for ( auto it2 = it1; it2 != short_slopes.end(); ++it2) {
            if (it1 == it2) { continue; }
            max_dist = max(max_dist, slope_dist(*it1, *it2));
        }    
    }
    slope ans(max_dist, num_slopes);
    fprintf(stdout, "%s,%d,%d", box.name.c_str(), max_dist, num_slopes);
    for ( auto it = short_slopes.begin(); it != short_slopes.end(); ++it) {
        fprintf(stdout, ",{%d,%d}", it->first, it->second);
    }
    fprintf(stdout, "\n");
    return ans;
}

int main(int argc, char** argv)
{
	Box box;
	for (const char* boxP = argv[1]; *boxP; ++boxP) {
		if (*boxP == '0') {
			box = box.child(0);
		} else if (*boxP == '1') {
			box = box.child(1);
		}
	}
    short_slope_dist_and_count(box);	
}

