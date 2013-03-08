#pragma once
#include "HeightMap.h"

class PerlinNoise: public HeightMap
{
public:
	int octave, biome;

	PerlinNoise(double persistence, int avgHeight, int type);
	void create();
	~PerlinNoise(void);

private:
	void interpolateMap(int delta);
};

