#include "HeightMap.h"

#pragma once
class VoronoiGraph: public HeightMap
{
public:
	int featurePoints;

	VoronoiGraph(int featurePoints);

	void create();

	~VoronoiGraph(void);
};

