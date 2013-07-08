/*
 *  MomData.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 11/10/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <string>
#include <vector>
#include <map>

using namespace std;
#include "CanonicalName.h"

void check(vector<string> relators, vector<pair<string, string> > triangles)
{
	for (vector<string>::iterator itR = relators.begin(); itR != relators.end(); ++itR)
	{
		map<string, int> valence;
		CanonicalName cn;
		cn.addRelator(*itR);
		vector< pair< string, string > >::iterator itT;
		for (itT = triangles.begin(); itT != triangles.end(); ++itT) {
			++valence[ cn.getCanonicalClass(itT->first) ];
			++valence[ cn.getCanonicalClass(itT->second) ];
			++valence[ cn.getCanonicalClass(cn.inverse(itT->first)) ];
			++valence[ cn.getCanonicalClass(cn.inverse(itT->first) + itT->second) ];
			++valence[ cn.getCanonicalClass(cn.inverse(itT->second)) ];
			++valence[ cn.getCanonicalClass(cn.inverse(itT->second) + itT->first) ];
		}
		printf("%s:\n", itR->c_str());
		map<string, int>::iterator it;
		for (it = valence.begin(); it != valence.end(); ++it) {
			printf("%s %d\n", it->first.c_str(), it->second);
		}
		printf("\n");
	}
}

int main(int argc, char** argv)
{
	
	char buf[10000];
	vector<string> relators;
	vector<pair<string, string> > triangles;
	while (fgets(buf, sizeof(buf), stdin)) {
		int n = strlen(buf);
		if (buf[n-1] == '\n')
			buf[n-1] = '\0';
		if (*buf == '\0')
			continue;
		char* pos = index(buf, ' ');
		if (pos) {
			*pos++ = '\0';
			triangles.push_back(make_pair(string(buf), string(pos)));
		} else {
			if (!triangles.empty()) {
				check(relators, triangles);
				relators.clear();
				triangles.clear();
			}
			relators.push_back(buf);
		}
	}
	if (!triangles.empty())
		check(relators, triangles);
}
