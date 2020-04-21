#ifndef __Box_h
#define __Box_h
#include "Params.h"
#include "ACJ.h"
#include "QuasiRelators.h"
#include <set>

struct Disk {
	Disk();
	std::string name;
	Disk child(int dir) const;
  XComplex center() const { return _center; }
  XComplex radius() const { return _radius; }
  ACJ c_ACJ() const { return _c_ACJ; }
  ACJ r_ACJ() const { return _r_ACJ; }
	bool operator< (const Disk& other) const {
		return name < other.name;
	}
  std::string desc() const;
private:
	int pos;
	double center_digits[2];
	double size_digits[2];
  double disk_center[2];
  double disk_size[2];
	XComplex _center; 
	XComplex _radius;
	ACJ _c_ACJ; // constant function
	ACJ _r_ACJ; // constant function
  void compute_center_and_radius();
};

/*
struct DiskNode {
	DiskNode() : left(nullptr), right(nullptr), test_index(-1), test_result(open) {}
	DiskNode *left;
	DiskNode *right;
	int test_index;
	box_state test_result;
  std::vector<std::string> pseudo_eliminators;
  bool is_complete();
  void clone_children();
};
*/

struct Box {
	Box();
	std::string name;
  std::set<Disk> e2_todo; 
  std::set<int> e2_killers; // test indices 
	QuasiRelators qr;
	Box child(int dir) const;
	Params<XComplex> center() const { return _center; }
	Params<ACJ> cover() const { return _cover; }
	Params<XComplex> nearer() const { return _nearer; } // returns all values closer to 0 than in box or 0 if box overlaps
	Params<XComplex> further() const { return _further; } // returns all values futher from 0 that in the box
	Params<XComplex> greater() const { return _greater; } // returns all values greater than in the box
  std::set<std::pair<double,double> > short_slopes;
private:
	int pos;
	double center_digits[6];
	double size_digits[6];
  double box_center[6];
  double box_size[6];
	Params<XComplex> _center;
	Params<ACJ> _cover;
	Params<XComplex> _nearer;
	Params<XComplex> _further;
	Params<XComplex> _greater;
  void compute_center_and_size();
	void compute_cover();
  void compute_nearer();
	void compute_further();
	void compute_greater();
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

inline const double areaUB(const Params<XComplex>&further)
{
    // Area is |lox_sqrt|^2*|Im(lattice)|.
    XComplex lox_sqrt = further.loxodromic_sqrt;
    double lat_im     = further.lattice.im;
    // Apply Lemma 7 of GMT.
    double lox_re = (1+EPS)*(lox_sqrt.re*lox_sqrt.re);
    double lox_im = (1+EPS)*(lox_sqrt.im*lox_sqrt.im);
    double lox_norm = (1+EPS)*(lox_re + lox_im);
    return (1+EPS)*(lox_norm*lat_im);
}

#endif // __Box_h
