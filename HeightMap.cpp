#include "stdafx.h"
#include "HeightMap.h"

double ** height;

HeightMap::HeightMap(void)
{
}

void HeightMap::init()
{
	height = (double **) malloc ((MAX_WORLD_SIZE) * sizeof(double *));
	if(!height)
	{
		log("Memory allocation error making height map\n");
		exit(1);
	}

	for(int k = 0; k <= MAX_WORLD_SIZE; k++)
	{
		height[k] = (double *) malloc ((MAX_WORLD_SIZE) * sizeof(double));
		if(!height[k])
		{
			log("Memory allocation error making height map\n");
			exit(1);
		}
	}

	for(int z = 0; z < MAX_WORLD_SIZE; z++)
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
			height[x][z] = 0;
}

//Returns the pointer to the height map
double * HeightMap::getMap(int x, int z)
{
	return &height[x][z];
}

HeightMap::~HeightMap(void)
{
}
