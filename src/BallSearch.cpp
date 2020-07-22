/*
 *  BallSearch.cpp
 *  horoballs
 *
 *  Created by Nathaniel Thurston on 23/04/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "BallSearch.h"
#include "CanonicalName.h"
#include "SL2C.h"
#include "Params.h"
#include <list>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
using namespace std;

namespace BallSearchImpl {
	struct Word {
		string name;
		string nameClass; // cuts off leading or tailing M,N,m,n products
		SL2C matrix;
	};
	
	struct LatticePoint {
		Params<XComplex> params;
		int x;
		int y;
		double distance;
	};
	
	struct LowerDistance : public binary_function<const LatticePoint&, const LatticePoint&, bool>
	{
		bool operator() (const LatticePoint& a, const LatticePoint& b)
		{
			return a.distance < b.distance;
		}
	};

	int gPower(Word w)
	{
		int gCount = 0;
		for (string::size_type pos = 0; pos < w.name.size(); ++pos)
			if (w.name[pos] == 'g' || w.name[pos] == 'G')
				++gCount;
		return gCount;
	}

	
	Word operator * (const Word& lhs, const LatticePoint& rhs); // Multiply word by lattice transtions
	Word operator * (const Word& lhs, const Word& rhs); // Multiply/concatiante two words
	
	struct WordPair {
		WordPair(Word& first, Word& second, Params<XComplex>& params);
		void setCurrentIndex(int i);
		
		Word firstWord;
		Word secondWord;
		vector<LatticePoint> distances;
		int distanceIndex;
		Word currentCombination;
		double currentBallSize;
	};
	
	struct SmallerBallSize : public binary_function<const WordPair&, const WordPair&, bool>
	{
		bool operator() (const WordPair& a, const WordPair& b)
		{
			return a.currentBallSize < b.currentBallSize;
		}
	};
	
	struct BallSearch {
		BallSearch(Params<XComplex> params_) :params(params_), G(constructG(params)), g(inverse(G)), m_foundBigBall(false) {}
		~BallSearch() {}
		void pushWord(Word word);
		void pushWord(string word);
		Word findWord();
		bool foundBigBall() { return m_foundBigBall; }
		void findNames(Word& word);
		void addRelator(string word);
		
		Params<XComplex> params;
		CanonicalName canonicalName;
		SL2C G;
		SL2C g;
		vector<Word> words;
		priority_queue<WordPair, vector<WordPair>, SmallerBallSize> pairs;
		bool m_foundBigBall;
	};
	
	void BallSearch::findNames(Word& word)
	{
		word.name = canonicalName.getCanonicalName(word.name);
		string::size_type firstG = word.name.find_first_of("gG");
		string::size_type lastG = word.name.find_last_of("gG");
		if (firstG == string::npos) {
			word.nameClass = "";
		} else {
			word.nameClass = word.name.substr(firstG, lastG - firstG + 1);
		}
	}
	
	Word inverse(Word& word)
	{
		Word inv;
		inv.name = word.name;
		reverse(inv.name.begin(), inv.name.end());
		string::size_type pos;
		for (pos = 0; pos < inv.name.size(); ++pos) {
			char c = inv.name[pos];
			if (c >= 'a' && c <= 'z')
				inv.name[pos] += 'A' - 'a';
			else
				inv.name[pos] -= 'A' - 'a';
		}
		inv.matrix = inverse(word.matrix);
		return inv;
	}
	
	Word operator * (const Word& lhs, const LatticePoint& rhs)
	{
		Word result;
		result.name = lhs.name;
		result.nameClass = lhs.nameClass;
		for (int i = 0; i <  rhs.x; ++i) result.name += "M";
		for (int i = 0; i < -rhs.x; ++i) result.name += "m";
		for (int i = 0; i <  rhs.y; ++i) result.name += "N";
		for (int i = 0; i < -rhs.y; ++i) result.name += "n";
		result.matrix = lhs.matrix * constructT(rhs.params, rhs.x, rhs.y);
		return result;
	}
	
	Word operator * (const Word& lhs, const Word& rhs)
	{
		Word result;
		result.name = lhs.name + rhs.name;
		result.matrix = lhs.matrix * rhs.matrix;
		return result;
	}

	WordPair::WordPair(Word& first, Word& second, Params<XComplex>& params)
		:firstWord(first), secondWord(second), distanceIndex(0)
	{
//        fprintf(stderr, "Buidling word pair\n");
//        fprintf(stderr, "a_1 = %f + %f i, c_1 = %f + %f i\n a_2 =  %f + %f i, c_2 = %f + %f i\n",
//                        firstWord.matrix.a.re, firstWord.matrix.a.im, firstWord.matrix.c.re, firstWord.matrix.c.im,
//                        secondWord.matrix.a.re, secondWord.matrix.a.im, secondWord.matrix.c.re, secondWord.matrix.c.im); 
		XComplex centerDiff = (firstWord.matrix.a / firstWord.matrix.c
		 - secondWord.matrix.a / secondWord.matrix.c).z;
        if (absUB(centerDiff) < infinity()) {
            int y0 = int(floor(centerDiff.im / params.lattice.im));
            int x0 = int(floor(centerDiff.re - y0*params.lattice.re));
//            fprintf(stderr, "%d, %d\n", x0, y0); 
            for (int x = -2-x0; x <= 3-x0; ++x) {
                for (int y = -1-y0; y <= 2-y0; ++y) {
                    // this is here to avoid having to reduce first^(-1) * second
                    if (x == 0 && y == 0 && firstWord.name[0] == secondWord.name[0])
                        continue;
                    LatticePoint l;
                    l.params = params;
                    l.x = -x;
                    l.y = -y;
                    XComplex t = (centerDiff + double(x) + double(y)*params.lattice).z;
                    l.distance = pow(absUB(t),2);
//                    fprintf(stderr, "%d, %d\n", -x, -y); 
                    if (abs(l.x) < 4 && abs(l.y) < 4) 
                        distances.push_back(l);
                }
            }
        }
		sort(distances.begin(), distances.end(), LowerDistance());
		setCurrentIndex(0);
	}
	
	void WordPair::setCurrentIndex(int i)
	{
		distanceIndex = i;
		if (i >= distances.size()) {
			currentBallSize = 0;
		} else {
			currentCombination = (inverse(firstWord) * distances[i]) * secondWord;
            // Looks like this is an heuristic ball size 
			currentBallSize = 0.5 / pow(absLB(currentCombination.matrix.c),2);
		}
//		printf("WordPair(%s,%s):setCurrentIndex(%d) : %d,%d %f %s size=%f\n",
//			firstWord->name.c_str(), secondWord->name.c_str(),
//			i, distances[i].x, distances[i].y, distances[i].distance,
//			currentCombination.name.c_str(), currentBallSize);
	}
	
	void BallSearch::pushWord(Word word)
	{
		vector<Word>::iterator it;
		// first, check to see if this word has already been pushed
		findNames(word);
		if (word.nameClass == "") {
			return;
        }
		for (it = words.begin(); it != words.end(); ++it) {
			if (it->nameClass == word.nameClass) {
				return;
            }
        }
//		fprintf(stderr,"pushWord(%s) class=%s\n", word.name.c_str(), word.nameClass.c_str());
		words.push_back(word);
		Word* wp = &words.back();
		for (it = words.begin(); it != words.end(); ++it) {
//            fprintf(stderr,"Trying to print\n");
//            fprintf(stderr, "First word %s, Second word %s\n", it->name.c_str(), word.name.c_str());
			WordPair pair(*it, word, params);
//			fprintf(stderr,"pushWordPair(%s,%s) distance size %d\n", pair.firstWord.name.c_str(), pair.secondWord.name.c_str(), pair.distances.size());
			if (pair.distances.size() > 0) {
				pairs.push(pair);
            }
		}
//        fprintf(stderr,"Done with pushWord\n");
	}
	
	void BallSearch::pushWord(string word)
	{
		Word w;
		w.name = word;
		int x = 0;
		int y = 0;
        string::reverse_iterator rit;
        for (rit = word.rbegin(); rit != word.rend(); ++rit) {
            char h = *rit;
            switch(h) {
				case 'm': --x; break;
				case 'M': ++x; break;
				case 'n': --y; break;
				case 'N': ++y; break;
				default: {
					if (x != 0 || y != 0) {
						w.matrix = constructT(params, x, y) * w.matrix;
						x = y = 0;
					}
					if (h == 'g') {
						w.matrix = g * w.matrix;
					} else if (h == 'G') {
						w.matrix = G * w.matrix;
					}
				}
			}
		}
        if (x != 0 || y != 0) {
            w.matrix = constructT(params, x, y) * w.matrix;
        }
//        XComplex a = w.matrix.a;
//        XComplex b = w.matrix.b;
//        XComplex c = w.matrix.c;
//        XComplex d = w.matrix.d;
//        fprintf(stderr, "Word: %s\n", w.name.c_str());
//        fprintf(stderr, "At the center is has coords\n");
//        fprintf(stderr, "a: %f + I %f\n", a.re, a.im);
//        fprintf(stderr, "b: %f + I %f\n", b.re, b.im);
//        fprintf(stderr, "c: %f + I %f\n", c.re, c.im);
//        fprintf(stderr, "d: %f + I %f\n", d.re, d.im);
		//printf("pushWord(%s): distance=%f\n", w.name.c_str(), 0.5 / norm(w.matrix.c));
		pushWord(w);
		Word wInv = inverse(w);
		findNames(wInv);
		pushWord(wInv);
		//fprintf(stderr, "pushed words %s(%s) and %s(%s)\n",
			//w.name.c_str(), w.nameClass.c_str(),
			//wInv.name.c_str(), wInv.nameClass.c_str());
	}
	
	Word BallSearch::findWord()
	{
		while (pairs.size() > 0) {
			WordPair largest(pairs.top());
			Word result(largest.currentCombination);
			string oldName = result.name;
			findNames(result);			
			pairs.pop();
			largest.setCurrentIndex(largest.distanceIndex+1);
			pairs.push(largest);
			vector<Word>::iterator it;
			for (it = words.begin(); it != words.end(); ++it)
				if (result.nameClass == it->nameClass)
					break;
			if (result.nameClass != "" && it == words.end()) {
				double sizeRatio = absLB(result.matrix.c) / absUB(words.front().matrix.c);
//				fprintf(stderr, "%s: sizeRatio = %f gPower = %d\n", result.name.c_str(), sizeRatio, gPower(result));
				if (sizeRatio < 1) {
					m_foundBigBall = true;
				}
				return result;
			}
			//fprintf(stderr, "rejecting %s^-1 *(%d,%d) %s = %s -> %s (%s)\n",
				//largest.firstWord->name.c_str(),
				//largest.distances[largest.distanceIndex-1].x,
				//largest.distances[largest.distanceIndex-1].y,
				//largest.secondWord->name.c_str(),
				//oldName.c_str(),
				//result.name.c_str(),
				//result.nameClass.c_str()
			//);
			//printf("duplicate; resorting\n");
		}
        return Word();
	}
	
	void BallSearch::addRelator(string w)
	{
		canonicalName.addRelator(w);
	}
}

vector<string> findWords(Params<XComplex> center, vector<string> seedWords, int numWords, int maxLength,
	vector<string> relators)
{
//    fprintf(stderr, "Params:\n");
//    fprintf(stderr, "L: %f + I %f \n", center.lattice.re, center.lattice.im);
//    fprintf(stderr, "S: %f + I %f \n", center.loxodromic_sqrt.re, center.loxodromic_sqrt.im);
//    fprintf(stderr, "P: %f + I %f \n", center.parabolic.re, center.parabolic.im);
	BallSearchImpl::BallSearch search(center);
//    fprintf(stderr, "Params:\n");
//    fprintf(stderr, "L: %f + I %f \n", search.params.lattice.re, search.params.lattice.im);
//    fprintf(stderr, "S: %f + I %f \n", search.params.loxodromic_sqrt.re, search.params.loxodromic_sqrt.im);
//    fprintf(stderr, "P: %f + I %f \n", search.params.parabolic.re, search.params.parabolic.im);
	for (int i = 0; i < relators.size(); ++i)
		if (relators[i].size() > 0)
			search.addRelator(relators[i]);
	for (int i = 0; i < seedWords.size(); ++i)
		if (seedWords[i].size() > 0)
			search.pushWord(seedWords[i]);
	search.pushWord("g");
	search.pushWord("G");
	vector<string> foundWords;
	while (numWords > int(foundWords.size()) || (-numWords > int(foundWords.size()) && !search.foundBigBall())) {
//        fprintf(stderr, "While loop\n");
		BallSearchImpl::Word w = search.findWord();
        int g_power = gPower(w);
		if (g_power == 0 || g_power > maxLength) {
			search.m_foundBigBall = false;
			continue;
		}
		//fprintf(stderr, "adding %s\n", w.name.c_str());
		search.pushWord(w);
		foundWords.push_back(w.nameClass);
	}
	return foundWords;
}

typedef struct {
    XComplex center;
    double height;
    SL2C gamma;
    string word;
} horoball; 

inline double sqnorm(const XComplex &x) {
    return x.re * x.re + x.im * x.im;
}

inline const XComplex conj(const XComplex &x) {
    return XComplex(x.re, -x.im);
}

inline XComplex mobius(const SL2C &x, const XComplex &p) {
    return (((x.a * p).z + x.b).z / ((x.c * p).z + x.d).z).z;
}

inline XComplex horo_center_inf(const SL2C &x) {
    if (absLB(x.c) == 0) {
        fprintf(stderr, "Transformation fixes infinity\n");
        return XComplex(1.0/0.0, 0.0);
    }
    return (x.a/x.c).z;
}

// Height of image of infinity horoball under x
double horo_image_height_inf(SL2C &x, double h) {
    if (absLB(x.c) == 0) {
        fprintf(stderr, "Transformation fixes infinity\n");
        return h;
    }
    return 1. / (h * sqnorm(x.c));
}

double horo_image_height_inf(SL2C &x, XComplex z, double h) {
    if (absLB(((x.c * z).z + x.d).z) != 0) {
        return h / sqnorm(((x.c * z).z + x.d).z);
    } else {
        // same as infinity horoball for inverse(x), but since only c matters, we don't bother
        return horo_image_height_inf(x, h);
    }
}


double g_height_cutoff = 0.05;
double g_eps = pow(2,-100);

inline double cusp_height(const Params<XComplex> params) {
    return 1. / absLB(params.loxodromic_sqrt);
}

inline double horo_center_cutoff(const Params<XComplex> params) {
    double cusp_h = cusp_height(params);
    return absUB(params.parabolic) +  sqrt(cusp_h/g_height_cutoff)/absLB(params.loxodromic_sqrt);
}

typedef pair<double,double> range;

inline range quad_sol(double a, double b, double c) {
    double d = b * b - 4. * a * c;
    double sq_d = sqrt(d);
    return range((-b - sq_d)/(2*a), (-b + sq_d)/(2*a));
}

#define MAX_SEEN_AGAIN 256
#define MAX_E2_SEEN_AGAIN 512

set<string> find_words(const Params<XComplex>& params, int num_words, int max_g_len, const vector<string>& relators,
                       bool e2_search, const map<string, int>& seen)
{
    CanonicalName canonicalName;
    set<string> new_words;
    if (e2_search) {
      new_words.insert("g");  
      new_words.insert("Mg");  
      new_words.insert("Ng");  
      new_words.insert("NMg");
      new_words.insert("G");
      new_words.insert("MG");
    }
    horoball I = { XComplex(0.0,0.0), 0.0, SL2C(), "" };  
    vector<horoball> level_zero;
    level_zero.push_back(I);
    map< int, vector<horoball> > horoballs;
    horoballs[0] = level_zero;
    // Generate new horoballs
    XComplex lattice = params.lattice;
    double cusp_h = cusp_height(params);
    int d = 0;
    int seen_count = 0;
    while (d < max_g_len) {
        vector<horoball> level;
        horoballs[d+1] = level;
        for ( const auto &ball : horoballs[d] ) {
            SL2C gamma = ball.gamma;
            double height = ball.height;
            string word = ball.word;
            char first = word[0];
            map<string,SL2C> valid;
            if ( first == 'g' ) {  
                valid["g"] = inverse(constructG(params));
            } else if ( first == 'G' ) {
                valid["G"] = constructG(params);
            } else {
                valid["G"] = constructG(params);
                valid["g"] = inverse(constructG(params));
            }
            // Apply G and g if possible
            for ( const auto &h : valid ) {
                SL2C h_gamma = h.second * gamma;
                XComplex h_center = horo_center_inf(h_gamma);

                // The pojection of h_center to the real axis along lattice
                // Recall that we assume -0.5 < real(lattice) < 0.5 and imag(lattice) > 0
                double N_len = h_center.im / lattice.im;
                double M_len = h_center.re - N_len * lattice.re;

                // Adjustments we make sure to land inside the fundamental domain 
                int N_pow, M_pow;
                if (abs(N_len) > pow(2.,-10) && abs(N_len - 1.0) > pow(2.,-10)) {
                    N_pow = -int(floor(N_len));
                } else {
                    N_pow = 0;
                }
                if (abs(M_len) > pow(2.,-10) && abs(M_len - 1.0) > pow(2.,-10)) {
                    M_pow = -int(floor(M_len));
                } else {
                    M_pow = 0;
                }

                // Keep track of the word and it's representative
                string h_word = h.first + word;
                double new_height = horo_image_height_inf(h_gamma, cusp_h);
                if (new_height > (1.15 * cusp_h) && !e2_search) {
                    if (find(relators.begin(), relators.end(), h_word) == relators.end()) { 
                        if (seen.find(h_word) == seen.end()) {
                            new_words.insert(h_word);
                        } else {
                            seen_count += 1;
                        }
                        // new_words.insert(canonicalName.canonical_mirror(h_word));
                        // printf("word %s\n", h_word.c_str());
                        if (new_words.size() >= num_words || seen_count > MAX_SEEN_AGAIN) {
                            return new_words;
                        }
                    }
                } else {
                    if (new_height < g_height_cutoff + g_eps) {
                        continue;
                    }
                    string M_word, N_word;
                    if (M_pow > 0) {
                        M_word = string(M_pow, 'M');
                    } else {
                        M_word = string(-M_pow, 'm');
                    }
                    if (N_pow > 0) {
                        N_word = string(N_pow, 'N');
                    } else {
                        N_word = string(-N_pow, 'n');
                    }
                    string new_word = N_word + M_word + h_word;
                    // fprintf(stderr, "New word generated %s\n", new_word.c_str());
                    SL2C T = constructT(params, M_pow, N_pow); 
                    SL2C new_gamma = T * h_gamma;
                    XComplex new_center = mobius(T, h_center);

                    horoball new_ball = { new_center, new_height, new_gamma, new_word };
                    horoballs[d+1].push_back(new_ball);

                    // e2 search wants many horoball in fundamental domain
                    if (e2_search) {
                        if (seen.find(new_word) == seen.end()) {
                            new_words.insert(new_word);
                        } else {
                            seen_count += 1;
                        }
                        if (new_words.size() >= num_words + 6 || seen_count > MAX_E2_SEEN_AGAIN) {
                            new_words.erase("g");  
                            new_words.erase("Mg");  
                            new_words.erase("Ng");  
                            new_words.erase("NMg");
                            new_words.erase("G");
                            new_words.erase("MG");
                            return new_words;
                        }
                    }

                    // Add translates that will be good for next depth
                    double x = new_center.re;
                    double y = new_center.im;
                    double r = horo_center_cutoff(params);
                    range h_range = quad_sol(1., 2.*x, sqnorm(new_center) - r * r);
                    range v_range = quad_sol(sqnorm(lattice), 2*(x * lattice.re + y * lattice.im), sqnorm(new_center) - r * r);
                    pair<int,int> horiz_range = range(int(floor(h_range.first)), int(ceil(h_range.second)));
                    pair<int,int> vert_range = range(int(floor(v_range.first)), int(ceil(v_range.second)));
                    for (int n = vert_range.first; n < vert_range.second; ++n) {
                        for (int m = horiz_range.first; m < horiz_range.second; ++m) {
                            if (m != 0 || n != 0) {
                                int shift_M_pow = M_pow + m;
                                int shift_N_pow = N_pow + n;
                                T = constructT(params, shift_M_pow, shift_N_pow);
                                XComplex shift_center = mobius(T, h_center);
                                // Make sure the horoball images in next depth are not too small
                                if (absLB(shift_center) < r) {
                                    SL2C shift_gamma = T * h_gamma;
                                    if (shift_M_pow > 0) {
                                        M_word = string(shift_M_pow, 'M');
                                    } else {
                                        M_word = string(-shift_M_pow, 'm');
                                    }
                                    if (shift_N_pow > 0) {
                                        N_word = string(shift_N_pow, 'N');
                                    } else {
                                        N_word = string(-shift_N_pow, 'n');
                                    }
                                    string shift_word = N_word + M_word + h_word;
                                    horoball shift_ball = { shift_center, new_height, shift_gamma, shift_word };
                                    horoballs[d+1].push_back(shift_ball);
                                    // e2 search wants many horoball in fundamental domain
                                    if (e2_search && abs(m) <= 1 && abs(n) <= 1) {
                                        if (seen.find(shift_word) == seen.end()) {
                                            new_words.insert(shift_word);
                                        } else {
                                            seen_count += 1;
                                        }
                                        if (new_words.size() >= num_words + 6 || seen_count > MAX_E2_SEEN_AGAIN) {
                                            new_words.erase("g");  
                                            new_words.erase("Mg");  
                                            new_words.erase("Ng");  
                                            new_words.erase("NMg");
                                            new_words.erase("G");
                                            new_words.erase("MG");
                                            return new_words;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        d += 1;
    }
    if (e2_search) {
      new_words.erase("g");  
      new_words.erase("Mg");  
      new_words.erase("Ng");  
      new_words.erase("NMg");
      new_words.erase("G");
      new_words.erase("MG");
    }
    return new_words;
}
