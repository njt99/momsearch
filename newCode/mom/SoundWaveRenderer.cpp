/*
 *  SoundWaveRenderer.cpp
 *  SoundWaves
 *
 *  Created by Nathaniel Thurston on 20/05/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

extern "C" {
#include "SoundWaveRenderer.h"
}
#include <math.h>
#import <vector>
#import <algorithm>

using namespace std;

double transducerLocation[][2] = {
	{0.31, 0.595},
	{0.1, 0.95},
	{0.15,0.95},
	{0.2, 0.95},
	{0.25,0.95},
	{0.3, 0.95},
	{0.35,0.95},
	{0.4, 0.95},
	{0.45,0.95},
	{0.5, 0.95},
	{0.55,0.95},
	{0.6, 0.95},
	{0.65,0.95},
	{0.7, 0.95},
	{0.75,0.95},
	{0.8, 0.95},
	{0.85,0.95},
	{0.9, 0.95}
};

struct Transducer {
	vector< vector< float > > distance;
	float startTime;
};

struct TPoint { int x; int y; float distance; };

bool operator < (const TPoint& a, const TPoint& b) {
	return a.distance < b.distance;
};

struct TNPoint { int n; int x; int y; float distance; };
bool operator < (const TNPoint& a, const TNPoint& b) {
	return a.distance < b.distance;
};

struct PhasePoint {
	PhasePoint() :s(0), c(0), intensity(0) {}
	float s;
	float c;
	float intensity;
};

void updateDistance(double& distance, double waveF, double edgeDistance, double cornerDistance)
{
	if (edgeDistance > 0 && cornerDistance > 0) {
		// newDist = waveF * sqrt(1 + t*t)
		// oldDist = t*cornerDistance + (1-t)*edgeDistance
		// total = waveF * sqrt(1 + t*t) + t*cornerDistance + (1-t)*edgeDistance
		// total' = waveF / sqrt(1 + t*t)
		double k = (edgeDistance - cornerDistance) / waveF;
		if (k > 0 && k < sqrt(0.5)) {
			double t = sqrt(k*k / (1 - k*k));
			double newDistance = waveF * hypot(1, t) + t*cornerDistance + (1-t) * edgeDistance;
//			printf("k = %f t = %f E = %f C = %f F = %f newDistance = %f\n",
//				k, t, edgeDistance, cornerDistance, waveF, newDistance);
			if (newDistance < distance)
				distance = newDistance;
		} else {
//			printf("k = %f out of range\n", k);
		}
		double newDistance = waveF + edgeDistance;
		if (newDistance < distance) {
//			printf("edge corner distance = %f\n", newDistance);
			distance = newDistance;
		}
		 newDistance = waveF*sqrt(2) + cornerDistance;
		if (newDistance < distance) {
//			printf("corner corner distance = %f\n", newDistance);
			distance = newDistance;
		}
	}
}

struct SoundWaveRenderer {
	SoundWaveRenderer(int width, int height,
		unsigned char* skullData_, int bytesPerRow, int bytesPerPixel);
	~SoundWaveRenderer();
	void draw(int numTransducers, float transducerPhase,
	unsigned char* bitmapData, int bytesPerRow, int bytesPerPixel);
	void initTransducer(int n);
	double waveLength(int x, int y);
	
private:
	int imageWidth;
	int imageHeight;
	unsigned char* skullData;
	int skullBytesPerRow;
	int skullBytesPerPixel;
	vector<Transducer> transducers;
	vector<TNPoint> initialPoints;
	vector< vector< PhasePoint > > pointPhase;
	int numPointsIncluded;
};

SoundWaveRenderer::SoundWaveRenderer(int width, int height, unsigned char* skullData_, int bytesPerRow, int bytesPerPixel)
{
	imageWidth = width;
	imageHeight = height;
	skullData = skullData_;
	skullBytesPerRow = bytesPerRow;
	skullBytesPerPixel = bytesPerPixel;
	for (int n = 0; n < NUM_TRANSDUCERS; ++n) {
		initTransducer(n);
	}
	
	pointPhase.resize(height);
	for (int i = 0; i < height; ++i)
		pointPhase[i].resize(width);
		
	float minStartTime = 0;
	for (int n = 0; n < NUM_TRANSDUCERS; ++n) {
		if (transducers[n].startTime < minStartTime)
			minStartTime = transducers[n].startTime;
	}
	for (int n = 0; n < NUM_TRANSDUCERS; ++n) {
		transducers[n].startTime -= minStartTime;
		for (int x = 0; x < imageWidth; ++x) {
			for (int y = 0; y < imageHeight; ++y) {
				TNPoint p;
				p.n = n;
				p.x = x;
				p.y = y;
				p.distance = transducers[n].startTime + transducers[n].distance[x][y];
				initialPoints.push_back(p);
			}
		}
		printf("startTime[%d] = %f\n", n, transducers[n].startTime);
	}
	sort(initialPoints.begin(), initialPoints.end());
	numPointsIncluded = 0;
}

int adj[8][2] = {
	{ 1, 0},
	{ 1, 1},
	{ 0, 1},
	{-1, 1},
	{-1, 0},
	{-1,-1},
	{ 0,-1},
	{ 1,-1}
};

double SoundWaveRenderer::waveLength(int x, int y)
{
	unsigned char* densityP = skullData + skullBytesPerRow*y + skullBytesPerPixel*x;
	double density = 1 + 0.5 * (*densityP) * (1.0 / 0xff);
	return imageHeight / (130.0 * density);
}

void SoundWaveRenderer::initTransducer(int n)
{
	transducers.resize(n+1);
	transducers[n].distance.resize(imageWidth);
	for (int x = 0; x < imageWidth; ++x)
		transducers[n].distance[x].resize(imageHeight, -1);
	float startX = transducerLocation[n][0] * imageWidth;
	float startY = transducerLocation[n][1] * imageHeight;
	transducers[n].startTime =  - transducers[0].distance[int(startX)][int(startY)];
	vector<TPoint> points;
	for (int x = 0; x < imageWidth; ++x) {
		for (int y = 0; y < imageHeight; ++y) {
			TPoint p;
			p.x = x;
			p.y = y;
			float dx = x - startX;
			float dy = y - startY;
			p.distance = dx*dx + dy*dy;
			points.push_back(p);
		}
	}
	sort(points.begin(), points.end());
	for (int i = 0; i < points.size(); ++i) {
		TPoint& p = points[i];
//		printf("processing %d,%d\n", p.x, p.y);
		double adjDistances[8];
		for (int j = 0; j < 8; ++j) {
			int adjX = p.x + adj[j][0];
			int adjY = p.y + adj[j][1];
			if (adjX >= 0 && adjX < imageWidth
			 && adjY >= 0 && adjY < imageHeight) {
				adjDistances[j] = transducers[n].distance[adjX][adjY];
			} else {
				adjDistances[j] = -1;
			}
		}
		double distance = 1e100;
		double f = 1 / waveLength(p.x, p.y);
		updateDistance(distance, f, adjDistances[0], adjDistances[1]);
		updateDistance(distance, f, adjDistances[2], adjDistances[1]);
		updateDistance(distance, f, adjDistances[2], adjDistances[3]);
		updateDistance(distance, f, adjDistances[4], adjDistances[3]);
		updateDistance(distance, f, adjDistances[4], adjDistances[5]);
		updateDistance(distance, f, adjDistances[6], adjDistances[5]);
		updateDistance(distance, f, adjDistances[6], adjDistances[7]);
		updateDistance(distance, f, adjDistances[0], adjDistances[7]);
		if (distance == 1e100) {
			distance = hypot(p.x - startX, p.y - startY) * f;
			printf("initial distance = %f\n", distance);
		}
		transducers[n].distance[p.x][p.y] = distance;
	}
	float clotX = transducerLocation[0][0] * imageWidth;
	float clotY = transducerLocation[0][1] * imageHeight;
	double clotDist = transducers[n].distance[int(clotX)][int(clotY)];
	printf("transducer[%d]: startTime = %f distance = %f diff=%f\n",
		n, transducers[n].startTime, clotDist, transducers[n].startTime + clotDist);
	transducers[n].startTime = -clotDist;
}

SoundWaveRenderer::~SoundWaveRenderer()
{
}

void SoundWaveRenderer::draw(int numTransducers, float transducerPhase,
	unsigned char* bitmapData, int bytesPerRow, int bytesPerPixel)
{
	// First, incorporate the new points
	float myDistance = transducerPhase - 100;
	while ( numPointsIncluded < initialPoints.size() && initialPoints[numPointsIncluded].distance < myDistance) {
		TNPoint& p = initialPoints[numPointsIncluded++];
		PhasePoint& pp = pointPhase[p.y][p.x];
		
		double f = 0.3 / transducers[p.n].distance[p.x][p.y];
		pp.s += f * sinf(p.distance);
		pp.c += f * cosf(p.distance);
		pp.intensity = hypot(pp.s, pp.c);
	}
	int clotX = int(transducerLocation[0][0] * imageWidth);
	int clotY = int(transducerLocation[0][1] * imageHeight);
	double phaseS = sin(transducerPhase);
	double phaseC = cos(transducerPhase);
	for (int j = 0; j < imageHeight; ++j) {
		unsigned char* destBase = bitmapData + bytesPerRow * j;
		unsigned char* srcBase = skullData + skullBytesPerRow * j;
		for (int i = 0; i < imageWidth; ++i) {
			unsigned char* destPixel = destBase + 3*i;
			unsigned char* srcPixel = srcBase + i;
			if (transducerPhase < 100) {
				double dist = transducerPhase - transducers[0].distance[i][j];
				if (dist < 0) dist = 0;
				*destPixel = ((*srcPixel) >> 2) + 0x7f + int(0x1f * sin(dist));
				*(destPixel+1) = ((*srcPixel) >> 2) + 0x7f;
				*(destPixel+2) = ((*srcPixel) >> 2) + 0x7f;
			} else {
				PhasePoint& p = pointPhase[j][i];
				double intensity = p.s * phaseS + p.c * phaseC;
				if (i == clotX && j == clotY)
					printf("clotIntensity = %f\n", intensity);
				*destPixel = ((*srcPixel) >> 2) + 0x7f + int(0x7f * intensity);
				*(destPixel+1) = ((*srcPixel) >> 2) + 0x7f;
				*(destPixel+2) = ((*srcPixel) >> 2) + 0x7f;
			}
		}
	}
	for (int n = 1; n < NUM_TRANSDUCERS; ++n) {
		int destX = int(transducerLocation[n][0] * imageWidth);
		int destY = int(transducerLocation[n][1] * imageHeight);
		for (int i = -2; i <= 2; ++i) {
			for (int j = -2; j <= 2; ++j) {
				unsigned char* destPixel = bitmapData + bytesPerRow * (j+destY) + bytesPerPixel * (i+destX);
				*destPixel = 0xff;
				*(destPixel+1) = 0xff;
				*(destPixel+2) = 0xff;
			}
		}
	}
}

SoundWaveRenderer* SoundWaveRenderer_create(int width, int height,
	unsigned char* skullData, int bytesPerRow, int bytesPerPixel)
{
	return new SoundWaveRenderer(width, height, skullData, bytesPerRow, bytesPerPixel);
}

void SoundWaveRenderer_destroy(SoundWaveRenderer *renderer)
{
	delete renderer;
}

void SoundWaveRenderer_draw(SoundWaveRenderer *renderer,
	int numTransducers, float transducerPhase,
	unsigned char* bitmapData, int bytesPerRow, int bytesPerPixel)
{
	renderer->draw(numTransducers, transducerPhase,
		bitmapData, bytesPerRow, bytesPerPixel);
}