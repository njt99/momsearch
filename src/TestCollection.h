#ifndef __TestCollection_h
#define __TestCollection_h
/*
 *  TestCollection.h
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */

#include <unordered_map>
#include <string>
#include <vector>
#include "Params.h"
#include "Box.h"

typedef enum _box_state
{
killed_bounds = 9,
killed_no_parabolics = 1,
killed_identity_impossible = 3,
killed_parabolics_impossible = 8,
killed_bad_parabolic = 4,
killed_elliptic = 5,
killed_failed_qr = 2,
open_with_qr = 6,
variety_nbd = 7,
killed_e2 = 15,
two_var_inter = 13,
variety_center = 10,
two_var_center = 14,
large_horoball_center = 11,
out_of_bounds_center = 12,
open = -1
} 
box_state;

struct ImpossibleRelations;

int g_length(std::string& w); 

struct TestCollection {
	int size();
	box_state evaluateCenter(int index, Box& box);
	box_state evaluateBox(int index, Box& box, std::string& aux_word, std::vector<std::string>& new_qrs,
                        std::unordered_map<int,ACJ>& para_cache,std::unordered_map<std::string,SL2ACJ>& words_cache);
	bool kills_disk_center(int index, const Disk& d,  const Box& box);
	bool kills_disk(int index, const Disk& d, const Box& box,
                  std::unordered_map<int,ACJ>& para_cache,std::unordered_map<std::string,SL2ACJ>& words_cache);
	const char* getName(int index);
	int add(std::string word);
	void load(const char* fileName);
	void loadImpossibleRelations(const char* fileName);
	SL2ACJ construct_word(std::string word, const Params<ACJ>& params,
                        std::unordered_map<int,ACJ>& para_cache,std::unordered_map<std::string,SL2ACJ>& words_cache);
	SL2ACJ construct_word_simple(std::string word, const Params<ACJ>& params);
	SL2C construct_word(std::string word, const Params<XComplex>& params);
private:
	std::map<std::string, int> stringIndex;
	std::vector<std::string> indexString;
	box_state evaluate_approx(std::string word, Params<XComplex>& params);
  box_state evaluate_ACJ(std::string word, Params<ACJ>& params, std::string& aux_word, std::vector<std::string>& new_qrs,
                         std::unordered_map<int,ACJ>& para_cache, std::unordered_map<std::string,SL2ACJ>& words_cache);
  bool ready_for_parabolics_test(SL2ACJ& w);
  bool only_bad_parabolics(SL2ACJ& w, Params<ACJ>& params);
  box_state is_var_intersection(Box& box, std::string& aux_word,
                         std::unordered_map<int,ACJ>& para_cache, std::unordered_map<std::string,SL2ACJ>& words_cache);
	ImpossibleRelations *impossible;
};

inline const bool maybe_variety(const SL2C& w) {
  return (absUB(w.c) < 1) && (absUB(w.b) < 1 || absLB(w.c) > 0);
}

inline const bool inside_var_nbd(const SL2ACJ& w) {
  return (absUB(w.c) < 1) && (absUB(w.b) < 1 || absLB(w.c) > 0);
}

inline const bool not_para_fix_inf(const SL2ACJ&x) {
  return absLB(x.c) > 0 
      || ((absLB(x.a-1) > 0 || absLB(x.d-1) > 0) && (absLB(x.a+1) > 0 || absLB(x.d+1) > 0));
}

inline const bool not_identity(const SL2ACJ&x) {
  return absLB(x.b) > 0 || not_para_fix_inf(x);
}

inline const bool maybe_large_horoball(const SL2C& w, const Params<XComplex>& params) {
  return absUB((w.c / params.loxodromic_sqrt).z) < 1;
}

inline bool large_horoball(SL2ACJ& w, const Params<ACJ>& params) {
  return absUB(w.c / params.loxodromic_sqrt) < 1;
}

#define ONE_OVER_E2_MIN 0.92593

inline bool disk_killed_by(const Disk& d, const SL2ACJ& w) {
  // Returns true if the disk is contained in the "kill radius"
  // of the horoball w(H_infty). A horoball of height h and center x
  // will interesect w(H_infty) if:
  //       h |S|/|c|^2 > dist(a/c - x)^2
  // We know that h >= 1/(|S| e_2^2), so an intersection is guaranteed if
  //      1/e_2 > dist(a - c x)
  // Since the disk has a radius, we need to make sure
  //      ONE_OVER_E2_MIN > (1+EPS)*(absUB(a - c center) + absUB(c radius))
  return (1+EPS)*(absUB(w.a - w.c * d.c_ACJ()) + absUB(w.c * d.r_ACJ())) < ONE_OVER_E2_MIN; 
}

inline bool disk_maybe_killed_by(const Disk& d, const SL2C& w) {
  // Returns true if the disk is contained in the "kill radius"
  // of the horoball w(H_infty) using SL2C.
  return absUB((w.a - (w.c * d.center()).z).z) + absUB((w.c * d.radius()).z) < ONE_OVER_E2_MIN; 
}

#endif // __TestCollection_h
