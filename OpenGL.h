#pragma once
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#pragma comment( lib, "vfw32.lib" )

#include "resource.h"
#include "TerrainGen.h"
#include "Camera.h"
#include "Actor.h"
#include "ActorManager.h"
#include "stdafx.h"
#include <stdarg.h>

TerrainGen world;
const int worldScale = 1;

GLvoid ResizeGLScene(GLsizei, GLsizei);
GLvoid KillGLWindow(GLvoid);
int DrawGLScene(GLvoid);
int InitGL(GLvoid);


void glPrint(const char *);
void checkLighting(bool lightSwitch);
void overlayDisplay();
void loadTexture(string path, int index);
void drawTerrainAsList();
void rasterStringToOverlay(string str);
void loadTextures();




typedef struct {									// Structure For Keyboard Stuff
	BOOL keyDown [256];								// Holds TRUE / FALSE For Each Key
} Keys;												// Keys

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
	Keys*				keys;						// Key Structure
	HWND				hWnd;						// Window Handle
	HDC					hDC;						// Device Context
	HGLRC				hRC;						// Rendering Context
	GL_WindowInit		init;						// Window Init
	BOOL				isVisible;					// Window Visible?
	DWORD				lastTickCount;				// Tick Counter
} GL_Window;										// GL_Window


void buildFont(GL_Window* window);
void perspective();
void TerminateApplication (GL_Window* window);		// Terminate The Application

void ToggleFullscreen (GL_Window* window);			// Toggle Fullscreen / Windowed Mode

BOOL RegisterWindowClass(Application * application);

BOOL CreateWindowGL(GL_Window* window);
BOOL DestroyWindowGL(GL_Window* window);

// These Are The Function You Must Provide
BOOL Initialize (GL_Window* window, Keys* keys);	// Performs All Your Initialization

void Deinitialize (void);							// Performs All Your DeInitialization

BOOL Update (DWORD milliseconds);					// Perform Motion Updates

void Draw (void);									// Perform All Your Scene Drawing

void rMouseSelection();								// Perform Selection
void lMouseSelection();
void ReshapeGL(int width, int height);
void clearCurrentSelection();

extern int mouse_x;
extern int mouse_y;
