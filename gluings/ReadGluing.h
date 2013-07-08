#ifndef _READGLUING_H
#define _READGLUING_H

/*
 * ReadGluing.H
 *
 * Functions to convert a line of text of the form
 *  "(6): 3 9 10 0 6 7 4 5 11 1 2 8"
 * To a TriangulationData structure usable by SnapPea.
 */

#include <string>
#include <vector>
extern "C" {
#include "snappea/SnapPea.h"
}

using namespace std;

enum PolyhedraTypes { _4=0, _33=1, _5=2, _34=3, _333=4,
		      _6=5, _35=6, _44=7, _334=8, _3333=9, _no_type = 1000 };

// A matching on 2n elements is represented as a list l of length 2n
// containing each integer from 0 to n-1 exactly once, and such that
// l(m)=r if and only if l(r)=m.
typedef vector<int> Matching;

// Read a line of text and determine the matching data and the
// polyhedra types. Returns false if it quits due to bad input (either
// formatting, or if the data doesn't represent a matching).
//   Input: line
//   Output: p_type, gluing
bool ConvertLine( string line, PolyhedraTypes & p_type, Matching & gluing );

// From the data returned by the previous function, fill out a
// TriangulationData structure. Currently returns false only if
// poly_type==_no_type.
//   Input: poly_type, gluing_data
//   Output: data
bool CreateTriangulation( PolyhedraTypes poly_type,
			  const Matching & gluing_data,
			  TriangulationData & data );

#endif // _READGLUING_H
