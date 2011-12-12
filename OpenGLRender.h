#pragma once
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#pragma comment( lib, "vfw32.lib" )

#include "stdafx.h"
#include "Camera.h"
#include "ActorManager.h"

class OpenGLRender
{
public:
	
	//typedef struct {									// Structure For Keyboard Stuff
	//	BOOL keyDown [256];								// Holds TRUE / FALSE For Each Key
	//} Keys;												// Keys
	//Keys*				keys;						// Key Structure

	bool keys[256];											// Keyboard routines
	bool active;										// Flag sets window to be active by default
	bool fullscreen;									// Flag sets window to be fullscreen by default
	bool light, lp, fp;

	//Texture Variables
	GLuint testTex[6];										//Array for holding OpenGL textures
	GLuint	*fontSet;										//Holds the OpenGL font data
	int cursorTex;
	GLuint terrainList;										//Terrain Display List

	//Object variables
	Camera *cam;
	ActorManager *manager;
	int screenH, screenW;									//User screen width and height
	int mouseX, mouseY;										//Mouse location
	int selectedID;			
	
	Vector3 target;

	//Debug display variables
	bool debug, debugP;
	double drawX, drawY;								
	int overlayLineCount;

	BOOL initialize (Vector2* windowSize);	// Performs All Your Initialization
	int drawGLScene(GLvoid);
	void overlayDisplay();
	void rasterStringToOverlay(string str);
	
	void drawTerrainAsList();
	void checkLighting(bool lightSwitch);
	void perspective();
	BOOL update (DWORD milliseconds, int mouseX, int mouseY);					// Perform Motion Updates
	void toggleLighting(bool lighting);
	void toggleDebug(bool debug);
	
	void loadTexture(string path, int index);
	void loadTextures();
	void glPrint(const char *fmt);

	OpenGLRender(Camera &c, ActorManager &m, GLuint &fSet);
	~OpenGLRender(void);
};

