// stdafx.cpp : source file that includes just the standard includes
// GL Test.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <sstream>

void log(string * text)
{
	// This function takes in a string pointer and prints that string to log.txt

	FILE * pFile = fopen("log.txt", "a");
	fputs(text->data(), pFile);
	fclose(pFile);
}

void log(string text)
{
	// This function takes in a string and prints that string to log.txt

	FILE * pFile = fopen("log.txt", "a");
	fputs(text.data(), pFile);
	fclose(pFile);
}

string dtos(double number)
{
	//This function takes in a double and returns that double as a string object

	std::ostringstream stream;
	stream << number;
	string str = stream.str();

	return str;
}

string itos(int number)
{
	//This function takes in an integer and returns that integer as a string object

	std::ostringstream stream;
	stream << number;
	string str = stream.str();

	return str;
}

double getRandomAsD(int randBound)
{
	//Will generate a random double between 0 and randBound
	return rand() % randBound;
}

int getRandomAsI(int randBound)
{
	//Will generate a random integer between 0 and randBound
	return (int)(rand() % randBound);
}

double findDistance(double startX, double startZ, double endX, double endZ)
{
	return sqrt(pow(endX - startX,2) + pow(endZ - startZ,2));
}

double cosineInterpolate(double a, double b, double x)
{
	//This function takes in two number values 'a' and 'b' and a variable x
	//that represents a normalized distance between the location of 'a' and 'b'.
	//	(Makes the assumption that the location of 'a' is closer to 0 than 'b')
	//Using the cosine trig function, an intermediate value between 'a' and 'b'
	//is returned. This intermediate value is closer to 'a' when 'x' is
	//closer to 0 and it is closer to 'b' when 'x' is closer to 1.

	double f = (1-cos(x * PI)) * .5;

	return a*(1-f)+b*f;
}

Vector3 crossProduct(Vector3 v1, Vector3 v2)
{
	Vector3 result;

	result.x = v1.y*v2.z - v1.z*v2.y;
	result.y = v1.z*v2.x - v1.x*v2.z;
	result.z = v1.x*v2.y - v1.y*v2.x;

	return result;
}

double dotProduct(Vector3 v1, Vector3 v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

void normalize(Vector3 &v)
{
	double length = sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));

	if(length == 0)
		return;		//Vector is zero vector

	v.x = v.x / length;
	v.y = v.y / length;
	v.z = v.z / length;
}

void setVector(Vector3 &v, double x, double y, double z)
{
	v.x = x;
	v.y = y;
	v.z = z;
}

void setCoord(Vector2 &v, double x, double y)
{
	v.x = x;
	v.y = y;
}


//This function takes in:
//		-A triange given by three 3d points in space
//		-A line given by two 3d points in space
//		-A target point
//
//This function determines if the line intersects with the triangle
//If they do intersect then values are given to the target vector on the exact coordinates that the line crosses the triangle
BOOL checkLineIntersect(Vector3 tp1, Vector3 tp2, Vector3 tp3, Vector3 lp1, Vector3 lp2, Vector3 &target)
{
	//tp1 must be the 90 degree angle in the triangle
	Vector3 n1, n2;				//Vectors for holding temporary data
	Vector3 intersectPos;		//Hit Point

	//n1 = tp2 - tp1
	//n2 = tp3 - tp1
	setVector(n1, tp2.x-tp1.x, tp2.y-tp1.y, tp2.z-tp1.z);
	setVector(n2, tp3.x-tp1.x, tp3.y-tp1.y, tp3.z-tp1.z);

	//Get normal vector for the triangle
	Vector3 normal = crossProduct(n1, n2);
	normalize(normal);

	//n1 = lp1 - tp1
	//n2 = lp2 - tp1
	setVector(n1, lp1.x-tp1.x, lp1.y-tp1.y, lp1.z-tp1.z);
	setVector(n2, lp2.x-tp1.x, lp2.y-tp1.y, lp2.z-tp1.z);

	//Calculate distance between the two points on the line and the plane
	double dist1 = dotProduct(n1, normal);
	double dist2 = dotProduct(n2, normal);

	if ( (dist1 * dist2) >= 0.0f) return FALSE;  // line doesn't cross the triangle.
	if ( dist1 == dist2) return FALSE;// line and plane are parallel

	//Find the point where the line intersects the plane
	//n1 = lp1 + (lp2-lp1) * ( -dist1/(dist2-dist1) )
	n1.x = lp1.x + (lp2.x-lp1.x)*(-dist1/(dist2-dist1));
	n1.y = lp1.y + (lp2.y-lp1.y)*(-dist1/(dist2-dist1));
	n1.z = lp1.z + (lp2.z-lp1.z)*(-dist1/(dist2-dist1));

	setVector(intersectPos, n1.x, n1.y, n1.z);

	//Find if the intersection point lies within all bounds of the triangle
	Vector3 vTest;
	setVector(n1, tp2.x-tp1.x, tp2.y-tp1.y, tp2.z-tp1.z);
	vTest = crossProduct(normal, n1);
	setVector(n1, intersectPos.x-tp1.x, intersectPos.y-tp1.y, intersectPos.z-tp1.z);
	if(dotProduct(vTest, n1) < 0) return FALSE;

	setVector(n1, tp3.x-tp2.x, tp3.y-tp2.y, tp3.z-tp2.z);
	vTest = crossProduct(normal, n1);
	setVector(n1, intersectPos.x-tp2.x, intersectPos.y-tp2.y, intersectPos.z-tp2.z);
	if(dotProduct(vTest, n1) < 0) return FALSE;

	setVector(n1, tp1.x-tp3.x, tp1.y-tp3.y, tp1.z-tp3.z);
	vTest = crossProduct(normal, n1);
	setVector(n1, intersectPos.x-tp1.x, intersectPos.y-tp1.y, intersectPos.z-tp1.z);
	if(dotProduct(vTest, n1) < 0) return FALSE;

	target =  intersectPos;

	return TRUE;
}