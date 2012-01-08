#pragma once
#include "PerlinNoise.h"
#include "VoronoiGraph.h"
#include "stdafx.h"

class TerrainGen
{
public:
	struct tile {
		int type;			//Type of terrain
		double height;		//Height at location
		float gradient;		//Slope
	};

	double pField, pForest, pDesert;		//Probabilities of each terrain type being generated (Must sum to 100)
	double persistence;
	int avgHeight;
	int waterHeight;		//Height above the lowest terrain point for water to be generated
	int snowHeight;		//Height below the highest terrain point snow to be generated
	int sandHeight;		//Height for shoreline to be generated
	int featurePoints;	//Detail parameter for the Voronoi Graph, a higher number creates larger and taller mountain structures
	double minHeight, maxHeight;

	double ** cTrack;		//Height values for the camera track
	tile ** terrain;		//Data storage for terrain
	nodePath ** nodes;
	int nodeSpread;

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