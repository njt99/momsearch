#include <set>
#include <vector>
#include <algorithm>
#include "QuasiRelators.h"
#include "Params.h"
using namespace std;

string QuasiRelators::getName(string w)
{
	NameStore::iterator it = names.find(w);
	if (it == names.end()) {
		addQuasiRelator(w);
		it = names.find(w);
	}
	return it->second;
}

void QuasiRelators::addQuasiRelator(string w)
{
	if (names.find(w) != names.end())
		return;
//  TODO We kill uniquenes as this causes problems when checking for ``large horoball'' failures in smaller boxes
//  This is because order of evaluations matters for us and changing the word can cause problems
//	Any validation of uniqueness will have to be done by code using qrs
    names[w] = w;
	nameVector.push_back(w);
}

string QuasiRelators::min_pow_desc()
{
  vector<string> qrs(nameVector);
  sort(qrs.begin(), qrs.end(), g_power_sort);
  int min_power = g_power(qrs.front());
	string buf;
	for (vector<string>::iterator it = qrs.begin(); it != qrs.end(); ++it) {
    if (g_power(*it) > min_power) {
       break;
    }
		if (!buf.empty()) {
      buf += ",";
    } 
		buf += *it;
  }
	return buf;
}

string QuasiRelators::desc()
{
	string buf;
	for (vector<string>::iterator it = nameVector.begin(); it != nameVector.end(); ++it) {
		if (!buf.empty())
			buf += ",";
		buf += *it;
	}
	return buf;
}

vector<string> QuasiRelators::words()
{
    return nameVector;
//	vector<string> result;
//	for (NameStore::iterator it = names.begin(); it != names.end(); ++it) {
//		result.push_back(it->first);
//	}
//	return result;
}

bool QuasiRelators::isQuasiRelator(std::string w)
{
	return names.find(w) != names.end();
}

string QuasiRelators::inverse(string w)
{
	reverse(w.begin(), w.end()); // reverse order of w
	
	// reverse case of w
	for (string::size_type p = 0; p < w.size(); ++p) {
		switch(w[p]) {
			case 'm': w[p] = 'M'; break;
			case 'M': w[p] = 'm'; break;
			case 'n': w[p] = 'N'; break;
			case 'N': w[p] = 'n'; break;
			case 'g': w[p] = 'G'; break;
			case 'G': w[p] = 'g'; break;
		}
	}

	// commute m and n so that m comes first
	for (bool progress = true; progress; progress = false) {
		for (string::size_type p = 0; p < w.size(); ++p) {
			if ((w[p  ] == 'n' || w[p  ] == 'N')
			 && (w[p+1] == 'm' || w[p+1] == 'M')) {
				char n = w[p];
				w[p] = w[p+1];
				w[p+1] = n;
				progress = true;
			}
		}
	}
	return w;
}
