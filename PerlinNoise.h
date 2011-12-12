#pragma once
#include "HeightMap.h"

class PerlinNoise: public HeightMap
{
public:

	PerlinNoise(double &p);
	void create(int octave);
	~PerlinNoise(void);

private:
	void interpolateMap(int delta);
};

