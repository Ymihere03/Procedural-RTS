#pragma once
class HeightMap
{
public:
	
	HeightMap(void);
	void init();
	double *getMap(int x, int z);
	~HeightMap(void);
};

