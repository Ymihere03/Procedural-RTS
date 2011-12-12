#pragma once
#include "PerlinNoise.h"
#include "VoronoiGraph.h"
#include "stdafx.h"

class TerrainGen
{
public:
	
	nodePath ** nodes;

	TerrainGen(void);
	
	void getColor(int x, int y);
	double * getTerrainHeight(int x, int z);
	int getTerrainType(int x, int z);
	double getTerrainGradient(int x, int z);
	double getSpecificTerrainHeight(double x, double z);
	double ** getCamTrack();
	void resetNodes();
	~TerrainGen(void);

private:
	void generateTerrain(int dX, int dZ);
	void modifyTerrainHeight();
	void perturbance(int variation);
	void combineHeightMapWithTerrain(HeightMap *pMap);
	void thermalErosion();
	void smooth(int x, int z, int spread);
	void findSlope(int x, int z);
	void setTerrainData(int x, int z);
	int terrainTypeDistribution();
	int interpolateType(int type1, int type2, double x);
	void makeCamTrack();
	double getProbabilityByType(int terrainType);
	
};