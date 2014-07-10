#pragma once
#include "Tank.h"

class Scout :
	public Tank
{
public:
	Scout(double x, double y, double z, int t);
	~Scout(void);
};

