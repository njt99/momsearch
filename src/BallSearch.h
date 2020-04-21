/*
 *  BallSearch.h
 *  horoballs
 *
 *  Created by Nathaniel Thurston on 23/04/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Params.h"

#include <vector>
#include <set>
#include <map>
#include <string>

std::vector<std::string> findWords(
	Params<XComplex> center,
	std::vector< std::string > seedWords,
	int numWords,
	int maxLength,
	std::vector< std::string > quasiRelators
);

std::set<std::string> find_words(const Params<XComplex>& params, int num_words, int max_g_len, const std::vector<std::string>& relators,
                                bool e2_search, const std::map<std::string, int>& seen);
