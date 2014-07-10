#pragma once
#include "Tank.h"

class Artillery :
	public Tank
{
public:
	Artillery(double x, double y, double z, int t);
	~Artillery(void);
};

