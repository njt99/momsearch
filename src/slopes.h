/*
 * slopes.h
 *
 * Defines interface for slope computations in search.
 * Currently focused on slope distance
 *
 */

#ifndef __slopes_h
#define __slopes_h
#include "Box.h"
#include <utility>

using namespace std;

const int short_slopes_max_dist(Box& box);
const int short_slopes_max_dist_center(Box& box);

#endif // __slopes_h
