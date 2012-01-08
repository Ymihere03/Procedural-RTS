#pragma once
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#pragma comment( lib, "vfw32.lib" )

#include "stdafx.h"
#include "Camera.h"
#include "ActorManager.h"
#include "TerrainManager.h"
#include "Simple OpenGL Image Library\src\SOIL.h"

class OpenGLRender
{
public:
	bool keys[256];											// Keyboard routines
	Vector3 target;											// Location of user clicks
	int clickWait;											//Time to wait before another click is accepted
	int clickWaitTotal;								//Total time to wait before another click is accepted


	//Object variables
	Camera *cam;
	ActorManager *aM;
	TerrainManager *tM;
	
	BOOL initialize (Vector2* windowSize);					// Performs All Your Initialization
	BOOL update (DWORD milliseconds, int mouseX, int mouseY);// Perform Motion Updates
	void perspective();
	int drawGLScene();

	OpenGLRender(Camera &c, ActorManager &a, TerrainManager &t, GLuint &fSet, int &sID);
	~OpenGLRender(void);

private:
	int screenH, screenW;									//User screen width and height
	int mouseX, mouseY;										//Mouse location
	int * selectedID;

	//Debug display variables
	bool debug, debugP;
	double drawX, drawY;								
	int overlayLineCount;
	int fpsUpdate, time;

	
	bool active;										// Flag sets window to be active by default
	bool fullscreen;									// Flag sets window to be fullscreen by default
	bool light, lp, fp;

	//Texture Variables
	GLuint testTex[7];										//Array for holding OpenGL textures
	GLuint	*fontSet;										//Holds the OpenGL font data
	int cursorTex;
	GLuint terrainList;										//Terrain Display List
	
	
	void overlayDisplay();
	void rasterStringToOverlay(string str);
	
	void fastDrawTerrain();
	void drawTerrain();
	void checkLighting(bool lightSwitch);
	
	
	void toggleLighting(bool lighting);
	void toggleDebug(bool debug);
	
	void loadTexture(string path, int index);
	void loadTextures();
	void glPrint(const char *fmt);
};

