#include "Actor.h"

//Vector3 *vertex, *normal;
//Vector2 *texture;
//int vCount=0, texCount=0, nCount=0;
//GLMmodel *model = NULL;

//Actor::Actor(char* path)
//{
	//filePath = "file";
	//path.copy(filePath, 20, 0);

	/*model = glmReadOBJ(path);
	if(!model)
		log("Cube failed to load\n");
	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 180.0);*/
//}

//Actor::Actor(double x, double y, double z, double xSize, double ySize, double zSize)
//{
//}

//Actor::Actor(void)
//{

//}

void Actor::setLocation(double x, double y, double z)
{
	setVector(location, x, y, z);
	setHitBox();
}

void Actor::setLocation(Vector3 &v)
{
	setVector(location, v.x, v.y, v.z);
	setHitBox();
}

void Actor::unselect()
{
	selected = false;
}

void Actor::setSelected(bool target)
{
	selected = target;
}

bool Actor::isSelected()
{
	return selected;
}

Vector3 Actor::getLocation()
{
	return location;
}

Vector3 Actor::getFacing()
{
	return facing;
}

double Actor::getFacingRads()
{
	return radianFacing;
}

void Actor::checkLifeTime(int time)
{
	lifeTime += time;
	if(lifeTime > totalLifeTime && totalLifeTime > 0)
		kill();
}


void Actor::init(string path)
{
	
	
	//Count the number of verticies, texture coordinates and vertex normals
	/*FILE * pFile = fopen(filePath.data(), "r");
	char line[50];
	
	while(!feof(pFile))
	{
		fgets(line, 50, pFile);
		switch(line[0])
		{
		case 'v':	//Valid line of data
			switch(line[1])
			{
			case ' ': vCount++; break;
			case 't': texCount++; break;
			case 'n': nCount++; break;
			default: break;
			}
			break;
		default:break;
		}
	}
	vertex = (Vector3 *) malloc ((vCount+1) * sizeof(Vector3));
	texture = (Vector2 *) malloc ((texCount+1) * sizeof(Vector2));
	normal = (Vector3 *) malloc ((nCount+1) * sizeof(Vector3));

	fclose(pFile);
	loadObjModel();*/
}



void Actor::loadObjModel()
{
	/*FILE * pFile = fopen(filePath.data(), "r");
	char line[50];
	char *token;
	double xNum, yNum, zNum;
	int vIndex=0, texIndex=0, nIndex=0;
	
	while(!feof(pFile))
	{
		fgets(line, 50, pFile);
		
		switch(line[0])
		{
		case 'v':	//Vertex Data
			switch(line[1])
			{
			case ' ':	//Vertex Location
				token = strtok(line, " ");

				token = strtok(NULL, " ");
				xNum = atof(token);
				token = strtok(NULL, " ");
				yNum = atof(token);
				token = strtok(NULL, " ");
				zNum = atof(token);

				setVector(vertex[vIndex], xNum, yNum, zNum);
				vIndex++;
				break;

			case 't':	//Texture coordinate
				token = strtok(line, " ");

				token = strtok(NULL, " ");
				xNum = atof(token);
				token = strtok(NULL, " ");
				yNum = atof(token);

				setCoord(texture[texIndex], xNum, yNum);
				texIndex++;
				break;

			case 'n':	//Vertex Normal
				token = strtok(line, " ");

				token = strtok(NULL, " ");
				xNum = atof(token);
				token = strtok(NULL, " ");
				yNum = atof(token);
				token = strtok(NULL, " ");
				zNum = atof(token);

				setVector(normal[nIndex], xNum, yNum, zNum);
				nIndex++;
				break;

			default:
				break;
			}

		default:
			break;
		}
	}


	fclose(pFile);*/
}

Actor::~Actor(void)
{
}
