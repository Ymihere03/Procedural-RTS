#pragma once
#include "HeightMap.h"

class PerlinNoise: public HeightMap
{
public:
	int octave;

	PerlinNoise(double persistence, int avgHeight);
	void create();
	~PerlinNoise(void);

private:
	void interpolateMap(int delta);
};

