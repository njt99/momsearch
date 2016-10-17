/*
 *  TestCollection.h
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */

#include <map>
#include <string>
#include <vector>
#include "Params.h"
#include "Box.h"

struct ImpossibleRelations;

struct TestCollection {
	int size();
	int evaluateCenter(int index, Box& box);
	int evaluateBox(int index, NamedBox& box);
	bool box_inside_nbd(NamedBox& box);
	bool box_inside_at_least_two_nbd(NamedBox& box);
	bool validIntersection(NamedBox& box);
	bool validVariety(std::string word, Box& box);
	bool validIdentity(std::string word, Box& box);
	const char* getName(int index);
	int add(std::string word);
	void load(const char* fileName);
	void loadImpossibleRelations(const char* fileName);
private:
	std::map<std::string, int> stringIndex;
	std::vector<std::string> indexString;
	
	SL2ACJ evaluate1(std::string word, Params<ACJ>& params);
	bool evaluate(std::string word, Params<XComplex>& params);
	bool large_horoball(std::string word, Params<ACJ>& params);
	int evaluate(std::string word, Params<ACJ>& params, bool notIdentity=false);
//	void enumerate(const char* w);
//	void enumerateTails(std::string s, int pCount, int lCount);
	ImpossibleRelations *impossible;
};

