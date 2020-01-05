/*
 * slopes.h
 *
 * Defines interface for slope computations in search.
 * Currently focused on slope distance
 *
 */

#include "slopes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <vector>

#define MAX_DEPTH 256
#define MAX_CODE_LEN 512

using namespace std;

typedef pair<int,int> slope;

int gcd(int u, int v) {
    while (v != 0) {
        int r = u % v;
        u = v;
        v = r;
    }
    return u;
}

const int slope_dist(slope s1, slope s2) {
    int dist = s1.first * s2.second - s1.second * s2.first;
    if (dist < 0) { return -dist; }
    else { return dist; }
}

const int short_slopes_max_dist(Box& box) {
    // Return max distance between short slopes over the box
    // Returns -1 in case of error
    vector< slope > short_slopes;
    Params<XComplex> nearer = box.nearer();
    if (nearer.lattice.im < 0) {
        fprintf(stderr, "Error: lattice with zero imaginary part in box %s\n", box.name.c_str());
        return -1;
    }
    if (fabs(nearer.lattice.re) > 0.5) {
        fprintf(stderr, "Error: abs(lattice real part) > 1/2 in box %s\n", box.name.c_str());
        return -1;
    }
    Params<ACJ> cover = box.cover();
    ACJ S = cover.loxodromic_sqrt;
    ACJ L = cover.lattice;
    ACJ SL = S*L;
    // fprintf(stderr, "Box: %s\n", box.name.c_str());
    double b = 0;
    // fprintf(stderr, "absUB(S*6) is %f\n", absUB(S * 6)); 
    while (absLB((S*nearer.lattice.im)*b) <= 6) {
        double a = 0;  
        // fprintf(stderr, "b is %d\n", (int) b);
        while ((1-EPS)*(absLB((S*a)*(S*a)) + absLB((SL*b)*(SL*b))) <= (1+EPS)*(36 + absUB((S*a)*(S*b)))) {
            // fprintf(stderr, "a is %d and b is %d\n", (int) a, (int) b);
            for (int sgn = -1; sgn < 2; sgn += 2) {
                ACJ g = ((L * b) + (sgn * a))*S;
                // avoid long slopes, non-primatives, and overconting (-1,0) and (1,0) or (0,1)
                if ((a == 0 && b == 0) || (sgn == -1 && (a == 0 || b == 0)) ||
                    gcd((int) a, (int) b) > 1 || absLB(g) > 6) {
                    continue;
                } 
                slope p((int) sgn*a, (int) b);
                short_slopes.push_back(p);
                if (short_slopes.size() > 8) { // speed up
                    return -1;
                }
            }
            a += 1;
        }
        b += 1;
        // fprintf(stderr, "absLB(b * nearer.lattice.im) is %f and  absUB(S*6) is %f\n", absLB(b * nearer.lattice.im), absUB(S * 6)); 
    }
    int num_slopes = short_slopes.size();
    int max_dist = 0;
    for ( auto it1 = short_slopes.begin(); it1 != short_slopes.end(); ++it1) {
        for ( auto it2 = it1; it2 != short_slopes.end(); ++it2) {
            if (it1 == it2) { continue; }
            max_dist = max(max_dist, slope_dist(*it1, *it2));
        }    
    }
    return max_dist;
}

const int short_slopes_max_dist_center(Box& box) {
    // Return max distance between short slopes over the box
    // Returns -1 in case of error
    vector< slope > short_slopes;
    Params<XComplex> center = box.center();
    if (center.lattice.im < 0) {
        return -1;
    }
    if (fabs(center.lattice.re) > 0.5) {
        return -1;
    }
    XComplex S = center.loxodromic_sqrt;
    XComplex L = center.lattice;
    XComplex SL = (S*L).z;
    // fprintf(stderr, "Box: %s\n", box.name.c_str());
    double b = 0;
    // fprintf(stderr, "absUB(S*6) is %f\n", absUB(S * 6)); 
    while (absLB(((S * L.im).z * b).z) <= 6) {
        double a = 0;  
        // fprintf(stderr, "b is %d\n", (int) b);
        while (absLB(((S*a).z * (S*a).z).z) + absLB(((SL*b).z * (SL*b).z).z) <= (36 + absUB(((S*a).z*(S*b).z).z))) {
            // fprintf(stderr, "a is %d and b is %d\n", (int) a, (int) b);
            for (int sgn = -1; sgn < 2; sgn += 2) {
                XComplex g = (((L * b).z + (sgn * a)).z * S).z;
                // avoid long slopes, non-primatives, and overconting (-1,0) and (1,0) or (0,1)
                if ((a == 0 && b == 0) || (sgn == -1 && (a == 0 || b == 0)) ||
                    gcd((int) a, (int) b) > 1 || absLB(g) > 6) {
                    continue;
                } 
                slope p((int) sgn*a, (int) b);
                short_slopes.push_back(p);
                if (short_slopes.size() > 8) { // speed up
                    return -1;
                }
            }
            a += 1;
        }
        b += 1;
        // fprintf(stderr, "absLB(b * nearer.lattice.im) is %f and  absUB(S*6) is %f\n", absLB(b * nearer.lattice.im), absUB(S * 6)); 
    }
    int num_slopes = short_slopes.size();
    int max_dist = 0;
    for ( auto it1 = short_slopes.begin(); it1 != short_slopes.end(); ++it1) {
        for ( auto it2 = it1; it2 != short_slopes.end(); ++it2) {
            if (it1 == it2) { continue; }
            max_dist = max(max_dist, slope_dist(*it1, *it2));
        }    
    }
    return max_dist;
}
