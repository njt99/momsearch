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

inline const ACJ sq(const ACJ& x) {
    return x*x;
}

inline const XComplex sq(const XComplex& x) {
    return (x*x).z;
}

void fill_slope_candidates(Box& box) {
    Params<XComplex> nearer = box.nearer();
    Params<ACJ> cover = box.cover();
    ACJ S = cover.loxodromic_sqrt;
    ACJ L = cover.lattice;
    ACJ SL = S*L;
    double b = 0;
    while (absLB((S*nearer.lattice.im)*b) <= 6) {
        double a = 0; 
        // want a*s <= b*s/2 + sqrt(36 - (b s lattice.im)^2) with s = |S|
        // unwrap to 2 a <= b or s^2 (2 a - b)^2 <= 144 - 4(b s lattice.im)^2
        double cutoff = (1-EPS)*(absLB(sq(S*(2*a - b))) + absLB(sq((S*nearer.lattice.im)*b*2)));
        while ( 2*a <= b || cutoff <= 144 ) { 
            for (double sgn = -1; sgn < 2; sgn += 2) {
                ACJ g = ((L * b) + (sgn * a))*S;
                // avoid long slopes, non-primatives, and overconting (-1,0) and (1,0) or (0,1)
                if ((a == 0 && b == 0) || (sgn == -1 && (a == 0 || b == 0)) ||
                    gcd(int(a), int(b)) > 1 || absLB(g) > 6) {
                    continue;
                } 
                slope p(sgn*a, b);
                box.short_slopes.insert(p);
            }
            a += 1;
            cutoff = (1-EPS)*(absLB(sq(S*(2*a - b))) + absLB(sq((S*nearer.lattice.im)*b)));
        }
        b += 1;
    }
}

const int short_slopes_max_dist(Box& box) {
    // Return max distance between short slopes over the box
    // Returns -1 in case of error
    Params<XComplex> nearer = box.nearer();
    Params<XComplex> greater = box.greater();
    if (greater.lattice.im < 0 || nearer.lattice.im <= 0) {
        fprintf(stderr, "Error: lattice with negative or zero imaginary part in box %s\n", box.name.c_str());
        return -1;
    }
    if (fabs(nearer.lattice.re) > 0.5) {
        fprintf(stderr, "Error: abs(lattice real part) > 1/2 in box %s\n", box.name.c_str());
        return -1;
    }
    Params<ACJ> cover = box.cover();
    ACJ S = cover.loxodromic_sqrt;
    ACJ L = cover.lattice;
    // Compute possible slopes once and then we will remove extras
    if (box.short_slopes.empty()) {
        fill_slope_candidates(box);
    }
    vector<slope> loc_short_slopes; 
    for (auto s_it = box.short_slopes.begin(); s_it != box.short_slopes.end(); ) {
        ACJ g = ((L * s_it->second) + s_it->first)*S;
        if (absLB(g) > 6) {
            s_it = box.short_slopes.erase(s_it);
        } else {
            loc_short_slopes.push_back(*s_it);
            if (loc_short_slopes.size() > 8) { // speed up
                return -1;
            }
            ++s_it;
        }
    }
    int max_dist = 0;
    for ( auto it1 = loc_short_slopes.begin(); it1 != loc_short_slopes.end(); ++it1) {
        for ( auto it2 = it1; it2 != loc_short_slopes.end(); ++it2) {
            if (it1 == it2) { continue; }
            max_dist = max(max_dist, slope_dist(*it1, *it2));
            if (max_dist > 5) { // speed up
                return -1;
            }
        }    
    }
    return max_dist;
}

const int short_slopes_max_dist_center(Box& box) {
    // Return max distance between short slopes over the box
    // Returns -1 in case of error
    Params<XComplex> center = box.center();
    if (center.lattice.im < 0) {
        return -1;
    }
    if (fabs(center.lattice.re) > 0.5) {
        return -1;
    }
    if (box.short_slopes.empty()) {
        fill_slope_candidates(box);
    }
    XComplex S = center.loxodromic_sqrt;
    XComplex L = center.lattice;
    vector<slope> loc_short_slopes; 
    for (auto s_it = box.short_slopes.begin(); s_it != box.short_slopes.end(); ++s_it) {
        XComplex g = (((L * s_it->second).z + s_it->first).z * S).z;
        if (absLB(g) <= 6) {
            loc_short_slopes.push_back(*s_it);
            if (loc_short_slopes.size() > 8) { // speed up
                return -1;
            }
        }
    }
    int max_dist = 0;
    for ( auto it1 = loc_short_slopes.begin(); it1 != loc_short_slopes.end(); ++it1) {
        for ( auto it2 = it1; it2 != loc_short_slopes.end(); ++it2) {
            if (it1 == it2) { continue; }
            max_dist = max(max_dist, slope_dist(*it1, *it2));
            if (max_dist > 5) {
                return -1;
            }
        }    
    }
    return max_dist;
}
