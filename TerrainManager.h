#pragma once
#include "stdafx.h"
#include "TerrainGen.h"
#include "Actor.h"
//#include "glext.h"

class TerrainManager
{
public:
	struct TerrainList
	{
		TerrainList *next;
		TerrainGen *world;
		GLuint list;
		Vector2 locationID;
	};

	TerrainList *root;
	//TerrainGen *world;
	GLuint * testTex;										//Array for holding OpenGL textures

	tile getTileData(int x, int z);
	double getHeight(int x, int z);
	int getType(int x, int z);
	double getSpecificHeight(double x, double z);
	void drawTerrain();

	void setMovePath(Actor *a, Vector3 &targetLocation);

	TerrainManager(void);
	~TerrainManager(void);

private:
	int seed;
	
	void writeTerrainToFile(TerrainGen *t, Vector2 location);
	void initTerrainVBO(TerrainList *world);
	void newChunk(int x, int z);
	void drawTerrainChunk(GLuint list, TerrainGen * world, Vector2 &location);

	void addToList(TerrainGen *w, Vector2 &location);
	void deleteFromList(Vector2 &locationID);
};

