/*
 *  SoundWaveRenderer.h
 *  SoundWaves
 *
 *  Created by Nathaniel Thurston on 20/05/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

typedef struct SoundWaveRenderer SoundWaveRenderer;
#define NUM_TRANSDUCERS 18
SoundWaveRenderer *SoundWaveRenderer_create(int width, int height,
	unsigned char* skullData, int bytgesPerRows, int bytesPerPixel);
void SoundWaveRenderer_destroy(SoundWaveRenderer *renderer);
void SoundWaveRenderer_draw(SoundWaveRenderer *renderer,
	int numTransducers, float transducerPhase,
	unsigned char* bitmapData, int bytesPerRow, int bytesPerPixel);