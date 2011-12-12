#include "StdAfx.h"
#include "VoronoiGraph.h"

Vector3 * FPlist;

VoronoiGraph::VoronoiGraph(int numPoints)
{
	init();		//Initialize memory for the height map

	FPlist = (Vector3 *) malloc ((numPoints+1) * sizeof(Vector3));

	for(int i = 0; i < numPoints; i++)
	{
		int x = getRandomAsI(MAX_WORLD_SIZE-1);
		int z = getRandomAsI(MAX_WORLD_SIZE-1);

		FPlist[i].x = x;
		FPlist[i].y = z;
		double r = getRandomAsI(100);

		if(r < 75)
			FPlist[i].z = 0;		//Cell will be drawn
		else
			FPlist[i].z = -1;		//Cell will not be drawn
	}

	for(int z = 0; z < MAX_WORLD_SIZE; z++)
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
		{
			double minDist = pow(FPlist[0].x-x, 2.0) + pow(FPlist[0].y-z, 2.0);
			double minDist2 = pow(FPlist[1].x-x, 2.0) + pow(FPlist[1].y-z, 2.0);
			int p = 0, p2 = 1;		//Keep track of the indicies that correspond to the minimum distance values

			for(int i = 2; i < numPoints; i++)
			{
				double newDist = pow(FPlist[i].x-x, 2.0) + pow(FPlist[i].y-z, 2.0);

				if(newDist < minDist)
				{
					p2 = p;
					minDist2 = minDist;

					p = i;
					minDist = newDist;
				}
				else if(newDist < minDist2)
				{
					p2 = i;
					minDist2 = newDist;
				}
			}
			*getMap(x, z) = (minDist2-minDist)/2300;
		}
}


VoronoiGraph::~VoronoiGraph(void)
{
}
