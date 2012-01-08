#pragma once
class HeightMap
{
public:
	double persistence;
	int avgHeight;

	HeightMap();
	void init();

	virtual void create()=0;

	double *getMap(int x, int z);
	~HeightMap(void);
};

