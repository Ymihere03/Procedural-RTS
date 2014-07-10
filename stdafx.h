// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

//Debugging memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG

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




#define MAX_WORLD_SIZE 129		//Max bounds for the world size
#define PI 3.1415927
#define GRAVITY -9.8

typedef struct{
	double x, y, z, a;
}	Vector4;

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

struct nodePath3d {
	Vector3 nodeData;
	nodePath3d *next;
};

typedef struct{
	float x, y, z;					//Position coordinates
	int type;						//Type of terrain
	float gradient;					//Slope
	Vector3 normal1, normal2;		//Normal Vector coordinates
	Vector3 texCoord1, texCoord2;	//Texture coordinates
} tile;

struct OverlayData
{
	Vector3 location;
	Vector3 color;
	int fontSet;
	string *text;
	int lifeTime;
	OverlayData *next;
};

#define BIOME_COUNT 5

enum terrainType {FIELD, FOREST, SAND, SNOW, WATER};
enum biomeType{PLAINS, MOUNTAINS, DESERT, FOREST_MIX, LAKES};

extern bool keys[256];		// Keyboard routines
extern int selectedID;	//Object ID selected by the user

void clearCurrentSelection();
void log(string * text);
void log(string text);
string dtos(double number);
string itos(int number);
double getDecimalRandom(int randBound);
double getRandomAsD(int randBound);
int getRandomAsI(int randBound);
bool choose(float probability);
double findDistance(double startX, double startZ, double endX, double endZ);
double find3dDistance(Vector3 v1, Vector3 v2);
double find3dDistance(Vector3 v);
double cosineInterpolate(double height1, double height2, double x);
Vector3 crossProduct(Vector3 v1, Vector3 v2);
double dotProduct(Vector3 v1, Vector3 v2);
void normalize(Vector3 &v);
void setVector(Vector4 &v, double x, double y, double z, double a);
void setVector(Vector3 &v, double x, double y, double z);
void setCoord(Vector2 &v, double x, double y);
BOOL checkLineIntersect(Vector3 tp1, Vector3 tp2, Vector3 tp3, Vector3 lp1, Vector3 lp2, Vector3 &intersect);


