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
		Vector3 normal1, normal2;
	};

			

	double ** cTrack;		//Height values for the camera track
	tile ** terrain;		//Data storage for terrain
	nodePath ** nodes;		//Data storage for path finding nodes
	int nodeSpread;			//Distance that nodes are seperated

	TerrainGen(int seed);
	
	void getColor(int x, int y);
	double * getTerrainHeight(int x, int z);
	int getTerrainType(int x, int z);
	double getTerrainGradient(int x, int z);
	double getSpecificTerrainHeight(double x, double z);
	Vector3 getN1(int x, int z);
	Vector3 getN2(int x, int z);
	double ** getCamTrack();
	void resetNodes();
	~TerrainGen(void);

private:
		int dX, dZ;			//Delta numbers used to denote the current generation resolution

	double pField, pForest, pDesert,		//Probabilities of each terrain type being generated (Must sum to 100)
		persistence;						//Terrain height variance
	int avgHeight,
		waterHeight,		//Height above the lowest terrain point for water to be generated
		snowHeight,		//Height below the highest terrain point snow to be generated
		sandHeight,		//Height for shoreline to be generated
		featurePoints;	//Detail parameter for the Voronoi Graph, a higher number creates larger and taller mountain structures
	double minHeight, maxHeight;


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