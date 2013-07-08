/*
 *  FindMomManifold.h
 *  mom
 *
 *  Created by Nathaniel Thurston on 17/10/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include "CanonicalName.h"
#include <string>
#include "Params.h"

struct FindMomManifold {
	struct Environment {
		FILE* momsFP; // map: relator -> mom(s)
		FILE* gluingsFP; // map: gluing -> manifold
	};
	
	virtual bool find(Params<AComplex1Jet>& params, std::string relators) = 0;
	
	static FindMomManifold* create(Environment *env);
};

