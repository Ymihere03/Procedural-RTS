// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

#include <time.h>
#include <math.h>


#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>




#define MAX_WORLD_SIZE 257		//Max bounds for the world size
#define PI 3.1415927
#define GRAVITY -.98



typedef struct{
	double x, y, z;
}	Vector3;

typedef struct{
	double x, y;
}	Vector2;

struct nodePath {
	Vector2 nodeData;
	double incentive;
	nodePath *next;
};

void log(string * text);
void log(string text);
string dtos(double number);
string itos(int number);
double getRandomAsD(int randBound);
int getRandomAsI(int randBound);
double findDistance(double startX, double startZ, double endX, double endZ);
double cosineInterpolate(double height1, double height2, double x);
Vector3 crossProduct(Vector3 v1, Vector3 v2);
double dotProduct(Vector3 v1, Vector3 v2);
void normalize(Vector3 &v);
void setVector(Vector3 &v, double x, double y, double z);
void setCoord(Vector2 &v, double x, double y);
BOOL checkLineIntersect(Vector3 tp1, Vector3 tp2, Vector3 tp3, Vector3 lp1, Vector3 lp2, Vector3 &target);


