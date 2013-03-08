#include "StdAfx.h"
#include "PerlinNoise.h"


//double ** height;

FILE * pFile;

//
//  FUNCTION: PerlinNoise() (Constructor)
//
//  PURPOSE: Allocates memory for the height map
//
//  COMMENTS:
//
PerlinNoise::PerlinNoise(double p, int aH, int b)
{
	biome = b;
	octave = 1;
	persistence = p;
	avgHeight = aH;
	init();		// Call to the parent class
}

//
//  FUNCTION: create()
//
//  PURPOSE: Creates one layer of the height map
//
//  COMMENTS:
//		Octave is the index of the current layer of height map being produced.
//		When octave is small, the map will have fewer data points
//		with a larger range of possible values. A large octave will 
//		yield more data points with a small range of possible values.
//		
//
void PerlinNoise::create()
{
	int frequency = (int)pow(2.0, octave);				//Number of points being used on each axis direction
	int delta = MAX_WORLD_SIZE/frequency;					//Distance between the points being used
	double amplitude_range = /**/pow(persistence, octave)*10000;		//Range of height values the points can take on
	double height;

	for(int z = 0; z < MAX_WORLD_SIZE; z += delta)
		for(int x = 0; x < MAX_WORLD_SIZE; x += delta)
		{
			//Picks a random number inside the height range by modding a large random number by the height range
			height = (rand() % (int)amplitude_range);

			switch(biome)
			{
				
				case FOREST_MIX:
				case PLAINS:
					if(getRandomAsI(100) < 10 && octave < 3)
						height *= 2;
					break;

				case LAKES:
					if(height > amplitude_range/2)
						height = -height + amplitude_range/2;
					else
						height += amplitude_range/2;

					if(getRandomAsI(100) < 5 && octave < 3)
						height += (getRandomAsD(6)-3)*height;
					break;

				case MOUNTAINS:
					
					//if(height > amplitude_range/2)
					//	height = -height + amplitude_range/2;
					//else
					//	height += amplitude_range/2;
					
					//if (height < persistence*10000*.3)
					//	height = persistence*10000*.1;
					
					//break;
					if(getRandomAsI(100) < 10 && octave < 2)
						height *= 3;
					break;
			}
			
			*getMap(x,z) = height/1000;
		}

	//After random points are generated, interpolate the map to fill in all the spaces
	interpolateMap(delta);
	octave++;
}

//
//  FUNCTION: interpolateMap()
//
//  PURPOSE: Interpolates a 2D height map
//
//  COMMENTS:
//		The height of any location is based off of the four nearest locations that
//		have already been given heights. This function uses the following logic to 
//		find the heights for the entire map:
//		
//		*---x1---*
//		|		 |
//		y1	O	 y2
//		|		 |
//		|		 |
//		*---x2---*
//		
//		To find the height of 'O', we are given the heights of the locations at all '*'.
//		Four intermediate interpolations are made to find the heights of x1, x2, y1, and y2.
//		Two more interpolations are calculated between x1, x2 and y1, y2 at location 'O'.
//		Those two results are averaged together and the final result is the final height of 'O'.
//

void PerlinNoise::interpolateMap(int delta)
{
	int lowX = 0, highX = delta;
	int lowZ = 0, highZ = delta;

	for(int z = 0; z < MAX_WORLD_SIZE; z++)
	{
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
		{
			//Adjust locations of known heights if necessary
			if(x > highX)
			{
				highX += delta;
				lowX += delta;
			}

			//Make sure the current location does not already have a height
			if(x == lowX && z == lowZ)
				continue;
			if(x == lowX && z == highZ)
				continue;
			if(x == highX && z == highZ)
				continue;
			if(x == highX && z == lowZ)
				continue;

			//Interpolate the four intermediate values
			double h1 = cosineInterpolate(*getMap(lowX, lowZ), *getMap(highX, lowZ), (x-lowX)/(double)delta);
			double h2 = cosineInterpolate(*getMap(lowX, highZ), *getMap(highX, highZ), (x-lowX)/(double)delta);
			double h3 = cosineInterpolate(*getMap(lowX, lowZ), *getMap(lowX, highZ), (z-lowZ)/(double)delta);
			double h4 = cosineInterpolate(*getMap(highX, lowZ), *getMap(highX, highZ), (z-lowZ)/(double)delta);

			//Interpolate the two values on the current location
			double h5 = cosineInterpolate(h1, h2, (z-lowZ)/(double)delta);
			double h6 = cosineInterpolate(h3, h4, (x-lowX)/(double)delta);

			//Set the height as the average
			*getMap(x, z) = (h5+h6)/2;
		}

		//Adjust locations of markers if necessary
		if(z >= highZ)
		{
			highZ += delta;
			lowZ += delta;
		}
		lowX = 0;
		highX = delta;
	}

}





PerlinNoise::~PerlinNoise(void)
{
}
