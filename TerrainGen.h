#pragma once
#include "PerlinNoise.h"
#include "VoronoiGraph.h"
#include "stdafx.h"

class TerrainGen
{
public:
	struct tile {
		float x, y, z;					//Position coordinates
		int type;						//Type of terrain
		float gradient;					//Slope
		Vector3 normal1, normal2;		//Normal Vector coordinates
		Vector3 texCoord1, texCoord2;	//Texture coordinates
	};

			

	double ** cTrack;		//Height values for the camera track
	tile ** terrain;		//Data storage for terrain
	nodePath ** nodes;		//Data storage for path finding nodes
	int nodeSpread;			//Distance that nodes are seperated

	TerrainGen(Vector2 location);
	
	void getColor(int x, int y);
	double getTerrainHeight(int x, int z);
	int getTerrainType(int x, int z);
	double getTerrainGradient(int x, int z);
	double getSpecificTerrainHeight(double x, double z);
	Vector3 getN1(int x, int z);
	Vector3 getN2(int x, int z);
	bool getTileDataFromFile(tile &t, Vector2 chunkLocation, int tileX, int tileZ);

	double ** getCamTrack();
	void resetNodes();
	~TerrainGen(void);

private:
		int dX, dZ;			//Delta numbers used to denote the current generation resolution

	Vector2 location;
	double pField, pForest, pDesert,		//Probabilities of each terrain type being generated (Must sum to 100)
		persistence;						//Terrain height variance
	int biome,			//Type of terrain to generate
		avgHeight,
		waterHeight,	//Height below this point will be water
		deepWater,		//Height below this point will be deep water
		snowHeight,		//Height above this point will be snow
		sandHeight,		//Height for shoreline to be generated
		featurePoints;	//Detail parameter for the Voronoi Graph, a higher number creates larger and taller mountain structures
	double minHeight, maxHeight;


	void generateTerrain(int dX, int dZ);
	void modifyTerrainHeight();
	void perturbance(int variation);
	void combineHeightMapWithTerrain(HeightMap *pMap, int iter);
	void thermalErosion();
	void smooth(int x, int z, int spread);
	void findSlope(int x, int z);
	void setTerrainData(int x, int z);
	int terrainTypeDistribution();
	int interpolateType(int type1, int type2, double x);
	void makeCamTrack();
	double getProbabilityByType(int terrainType);
	
	
};