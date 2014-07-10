#pragma once
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glew32.lib")
#pragma comment( lib, "glu32.lib" )
#pragma comment( lib, "vfw32.lib" )

#include <stdarg.h>
#include "stdafx.h"
#include "resource.h"

#include "OpenGLRender.h"
#include "TerrainManager.h"
#include "TerrainGen.h"
#include "Camera.h"
#include "Actor.h"
#include "ActorManager.h"
#include "GameManager.h"

//TerrainGen *world;
OpenGLRender *glRender;
TerrainManager *terrainManager;
//TerrainGen *world;
ActorManager *actorManager;
GameManager *gameManager;
Camera *camera;											//Camera object that holds orientation values for camera
bool keys[256];											// Keyboard routines
int selectedID;		//Object ID selected by the user

GLvoid ResizeGLScene(GLsizei, GLsizei);
GLvoid KillGLWindow(GLvoid);
//int DrawGLScene(GLvoid);


void glPrint(const char *);
//void checkLighting(bool lightSwitch);
//void overlayDisplay();
void loadTexture(string path, int index);
//void drawTerrainAsList();
//void rasterStringToOverlay(string str);
void loadTextures();





typedef struct {									// Contains Information Vital To Applications
	HINSTANCE*		hInstance;						// Application Instance
	const char*		className;						// Application ClassName
} Application;										// Application

typedef struct {									// Window Creation Info
	Application*		application;				// Application Structure
	char*				title;						// Window Title
	int					width;						// Width
	int					height;						// Height
	int					bitsPerPixel;				// Bits Per Pixel
	BOOL				isFullScreen;				// FullScreen?
} GL_WindowInit;									// GL_WindowInit

typedef struct {									// Contains Information Vital To A Window
	HWND				hWnd;						// Window Handle
	HDC					hDC;						// Device Context
	HGLRC				hRC;						// Rendering Context
	GL_WindowInit		init;						// Window Init
	BOOL				isVisible;					// Window Visible?
	DWORD				lastTickCount;				// Tick Counter
} GL_Window;										// GL_Window

GL_Window  window;
GLuint *fonts;
//Keys*       g_keys;

void buildFont(GLuint &list, LPCWSTR fontName, int size);
//void perspective();
void TerminateApplication ();		// Terminate The Application

void ToggleFullscreen ();			// Toggle Fullscreen / Windowed Mode

BOOL RegisterWindowClass(Application * application);

BOOL CreateWindowGL();
BOOL DestroyWindowGL();

// These Are The Function You Must Provide


void Deinitialize (void);							// Performs All Your DeInitialization



void Draw (void);									// Perform All Your Scene Drawing

void rMouseSelection();								// Perform Selection
void lMouseSelection();
void ReshapeGL(int width, int height);
void clearCurrentSelection();

extern int mouse_x;
extern int mouse_y;
