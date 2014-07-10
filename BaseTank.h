#pragma once
#include "Tank.h"
class BaseTank :
	public Tank
{
public:
	BaseTank(double x, double y, double z, int t);
	~BaseTank(void);
};

