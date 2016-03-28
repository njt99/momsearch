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
#include <string>

std::vector<std::string> findWords(
	Params<XComplex> center,
	std::vector< std::string > seedWords,
	int numWords,
	int maxLength,
	std::vector< std::string > quasiRelators
);
