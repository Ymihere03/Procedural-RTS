/* GL Test.cpp : Defines the entry point for the application.
 * 
 * This class is responsible for all Open GL commands and displaying OpenGL on the screen
 */

#include "OpenGL.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE* hInst;										// current instance
TCHAR szTitle[MAX_LOADSTRING];							// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];					// the main window class name
//GL_Window*  g_window;
//Keys*       g_keys;

#define WM_TOGGLEFULLSCREEN (WM_USER+1)					// Application Define Message For Toggling
														// Between Fullscreen / Windowed Mode
static BOOL g_isProgramLooping;							// Window Creation Loop, For FullScreen/Windowed Toggle																		// Between Fullscreen / Windowed Mode
static BOOL g_createFullScreen = TRUE;							// If TRUE, Then Create Fullscreen

int	mouse_x, mouse_y;									// The Current Position Of The Mouse
int newMouseX = -1;
int timeElapsed;

// OpenGL variables

HGLRC hRC=NULL;											// Permanent Rendering Context
HDC  hDC=NULL;											// Private GDI Device Context
HWND hWnd=NULL;											// Holds the Window Handle
int screenH, screenW;
Vector3 clickRay1, clickRay2;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//This function initializes the main window of the program and adds OpenGL to the window
	//so it can be displayed. The following things are initialized:
	//	Terrain generator
	//	OpenGL camera variables
	//	Screen dimensions/fullscreen
	//	Main window design/capabilities
	//	Pixel Format
	//	OpenGL
	//
	//The main loop is entered after initialization

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GLTEST, szWindowClass, MAX_LOADSTRING);

	Application			application;									// Application Structure
	//GL_Window			window;											// Window Structure
	BOOL				isMessagePumpActive;							// Message Pump Active?
	MSG					msg;											// Window Message Structure
	DWORD				tickCount;										// Used For The Tick Counter
	hInst				= &hInstance;

	// Fill Out Application Data
	application.className = "OpenGL";									// Application Class Name
	application.hInstance = hInst;									// Application Instance

	//Initialize world objects
	//world = new TerrainGen();
	
	//Clear out the log file
	FILE * pFile = fopen("log.txt", "w");
	fclose(pFile);

	//Enable memory leak debugging
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	fonts = (GLuint *) malloc (2*sizeof(GLuint));
	
	//Initialize Core program components
	terrainManager = new TerrainManager();
	actorManager = new ActorManager(terrainManager);
	camera = new Camera(terrainManager->root->world->getCamTrack());
	gameManager = new GameManager(actorManager, terrainManager);
	glRender = new OpenGLRender(*camera, *actorManager, *terrainManager, *gameManager, *fonts);
	
	selectedID = -1;
	for(int i = 0; i < 256; i++)
		keys[i] = false;

	// Fill Out Window
	ZeroMemory (&window, sizeof (GL_Window));							// Make Sure Memory Is Zeroed
	window.init.application		= &application;							// Window Application
	window.init.title			= "NeHe's Picking Tutorial";			// Window Title
	window.init.width			= 1680;									// Window Width
	window.init.height			= 1050;									// Window Height
	screenW = window.init.width;
	screenH = window.init.height;
	window.init.bitsPerPixel	= 16;									// Bits Per Pixel
	window.init.isFullScreen	= TRUE;									// Fullscreen? (Set To TRUE)

	if (MessageBox (HWND_DESKTOP, L"Would You Like To Run In Fullscreen Mode?", L"Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		window.init.isFullScreen = FALSE;								// If Not, Run In Windowed Mode
		window.init.width			= 640;									// Window Width
		window.init.height			= 480;									// Window Height
		screenW = window.init.width;
		screenH = window.init.height;
	}

	// Register A Class For Our Window To Use
	if (RegisterWindowClass (&application) == FALSE)					// Did Registering A Class Fail?
	{
		// Failure
		MessageBox (HWND_DESKTOP, L"Error Registering Window Class!", L"Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;														// Terminate Application
	}

	// Create A Window
		//window.init.isFullScreen = g_createFullScreen;					// Set Init Param Of Window Creation To Fullscreen?
		if (CreateWindowGL () == TRUE)							// Was Window Creation Successful?
		{
			ReshapeGL(window.init.width, window.init.height);
			Vector2 size;
			setCoord(size, window.init.width, window.init.height);
			if (glRender->initialize (&size) == FALSE)					// Call User Intialization
			{
				// Failure
				TerminateApplication();							// Close Window, This Will Handle The Shutdown
			}
			else
			{
				buildFont(fonts[0], L"Courier New", 14);
				buildFont(fonts[1], L"Times", 14);
				isMessagePumpActive = TRUE;
				while (isMessagePumpActive == TRUE)						// While The Message Pump Is Active
				{
					// Success Creating Window.  Check For Window Messages
					if (PeekMessage (&msg, window.hWnd, 0, 0, PM_REMOVE) != 0)
					{
						// Check For WM_QUIT Message
						if (msg.message != WM_QUIT)						// Is The Message A WM_QUIT Message?
						{
							DispatchMessage (&msg);						// If Not, Dispatch The Message
						}
						else											// Otherwise (If Message Is WM_QUIT)
						{
							isMessagePumpActive = FALSE;				// Terminate The Message Pump
						}
					}
					else												// If There Are No Messages
					{
						if (window.isVisible == FALSE)					// If Window Is Not Visible
						{
							WaitMessage ();								// Application Is Minimized Wait For A Message
						}
						else											// If Window Is Visible
						{
							//----  Main Process Loop  ----//
							tickCount = GetTickCount ();				// Get The Tick Count
							if(!glRender->update (tickCount - window.lastTickCount, mouse_x, mouse_y))	// Update The Counter
								TerminateApplication();
							timeElapsed = tickCount - window.lastTickCount;
							window.lastTickCount = tickCount;			// Set Last Count To Current Count
							
							actorManager->updateObjects(timeElapsed);
							gameManager->update();
							glRender->drawGLScene();									// Draw the Scene
							
							SwapBuffers (window.hDC);					// Swap Buffers (Double Buffering)
						}
					}
				}
			}
			// Application Is Finished
			//Deinitialize ();											// User Defined DeInitialization

			DestroyWindowGL ();									// Destroy The Active Window
		}
		else															// If Window Creation Failed
		{
			// Error Creating Window
			MessageBox (HWND_DESKTOP, L"Error Creating OpenGL Window", L"Error", MB_OK | MB_ICONEXCLAMATION);
			g_isProgramLooping = FALSE;									// Terminate The Loop
		}
}






//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------


void TerminateApplication ()							// Terminate The Application
{
	PostMessage (window.hWnd, WM_QUIT, 0, 0);							// Send A WM_QUIT Message
	g_isProgramLooping = FALSE;											// Stop Looping Of The Program
}

void ToggleFullscreen ()								// Toggle Fullscreen/Windowed
{
	PostMessage (window.hWnd, WM_TOGGLEFULLSCREEN, 0, 0);				// Send A WM_TOGGLEFULLSCREEN Message
}

void ReshapeGL (int width, int height)									// Reshape The Window When It's Moved Or Resized
{
	glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));				// Reset The Current Viewport

	glRender->perspective();
}

BOOL ChangeScreenResolution (int width, int height, int bitsPerPixel)	// Change The Screen Resolution
{
	DEVMODE dmScreenSettings;											// Device Mode
	ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));					// Make Sure Memory Is Cleared
	dmScreenSettings.dmSize				= sizeof (DEVMODE);				// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth		= width;						// Select Screen Width
	dmScreenSettings.dmPelsHeight		= height;						// Select Screen Height
	dmScreenSettings.dmBitsPerPel		= bitsPerPixel;					// Select Bits Per Pixel
	dmScreenSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;


	if (ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		return FALSE;													// Display Change Failed, Return False
	}
	return TRUE;														// Display Change Was Successful, Return True
}

BOOL RegisterWindowClass (Application* application)						// Register A Window Class For This Application.
{																		// TRUE If Successful
	// Register A Window Class
	WNDCLASSEX windowClass;												// Window Class
	ZeroMemory (&windowClass, sizeof (WNDCLASSEX));						// Make Sure Memory Is Cleared
	windowClass.cbSize			= sizeof (WNDCLASSEX);					// Size Of The windowClass Structure
	windowClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraws The Window For Any Movement / Resizing
	windowClass.lpfnWndProc		= WndProc;							// WindowProc Handles Messages
	windowClass.cbClsExtra		= 0;
	windowClass.cbWndExtra		= 0;
	windowClass.hInstance		= *hInst;				// Set The Instance
	windowClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hbrBackground	= NULL;									// Class Background Brush Color
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	windowClass.lpszClassName	= szWindowClass;				// Sets The Applications Classname
	
	if (!RegisterClassEx (&windowClass))							// Did Registering The Class Fail?
	{
		// NOTE: Failure, Should Never Happen
		MessageBox (HWND_DESKTOP, L"RegisterClassEx Failed!", L"Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;													// Return False (Failure)
	}
	return TRUE;														// Return True (Success)
}

BOOL CreateWindowGL ()			// This Code Creates Our OpenGL Window
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;							// Define Our Window Style
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;						// Define The Window's Extended Style

	ShowCursor(FALSE);
	PIXELFORMATDESCRIPTOR pfd =											// pfd Tells Windows How We Want Things To Be
	{
		sizeof (PIXELFORMATDESCRIPTOR),									// Size Of This Pixel Format Descriptor
		1,																// Version Number
		PFD_DRAW_TO_WINDOW |											// Format Must Support Window
		PFD_SUPPORT_OPENGL |											// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,												// Must Support Double Buffering
		PFD_TYPE_RGBA,													// Request An RGBA Format
		window.init.bitsPerPixel,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,												// Color Bits Ignored
		0,																// No Alpha Buffer
		0,																// Shift Bit Ignored
		0,																// No Accumulation Buffer
		0, 0, 0, 0,														// Accumulation Bits Ignored
		16,																// 16Bit Z-Buffer (Depth Buffer)  
		0,																// No Stencil Buffer
		0,																// No Auxiliary Buffer
		PFD_MAIN_PLANE,													// Main Drawing Layer
		0,																// Reserved
		0, 0, 0															// Layer Masks Ignored
	};

	RECT windowRect = {0, 0, window.init.width, window.init.height};	// Define Our Window Coordinates

	GLuint PixelFormat;													// Will Hold The Selected Pixel Format

	if (window.init.isFullScreen == TRUE)								// Fullscreen Requested, Try Changing Video Modes
	{
		if (ChangeScreenResolution (window.init.width, window.init.height, window.init.bitsPerPixel) == FALSE)
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			MessageBox (HWND_DESKTOP, L"Mode Switch Failed.\nRunning In Windowed Mode.", L"Error", MB_OK | MB_ICONEXCLAMATION);
			window.init.isFullScreen = FALSE;							// Set isFullscreen To False (Windowed Mode)
		}
		else															// Otherwise (If Fullscreen Mode Was Successful)
		{
			windowStyle = WS_POPUP;										// Set The WindowStyle To WS_POPUP (Popup Window)
			//windowExtendedStyle |= WS_EX_TOPMOST;						// Set The Extended Window Style To WS_EX_TOPMOST
		}																// (Top Window Covering Everything Else)
	}
	else																// If Fullscreen Was Not Selected
	{
		// Adjust Window, Account For Window Borders
		AdjustWindowRectEx (&windowRect, windowStyle, 0, windowExtendedStyle);
	}

	// Create The OpenGL Window
	window.hWnd = CreateWindowEx (windowExtendedStyle,					// Extended Style
								   szWindowClass,						// Class Name
								   szTitle,					// Window Title
								   windowStyle,							// Window Style
								   0, 0,								// Window X,Y Position
								   windowRect.right - windowRect.left,	// Window Width
								   windowRect.bottom - windowRect.top,	// Window Height
								   HWND_DESKTOP,						// Desktop Is Window's Parent
								   0,									// No Menu
								   *window.init.application->hInstance, // Pass The Window Instance
								   &window);

	if (window.hWnd == 0)												// Was Window Creation A Success?
	{
		return FALSE;													// If Not Return False
	}

	window.hDC = GetDC (window.hWnd);									// Grab A Device Context For This Window
	if (window.hDC == 0)												// Did We Get A Device Context?
	{
		// Failed
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	PixelFormat = ChoosePixelFormat (window.hDC, &pfd);				// Find A Compatible Pixel Format
	if (PixelFormat == 0)												// Did We Find A Compatible Format?
	{
		// Failed
		ReleaseDC (window.hWnd, window.hDC);							// Release Our Device Context
		window.hDC = 0;												// Zero The Device Context
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	if (SetPixelFormat (window.hDC, PixelFormat, &pfd) == FALSE)		// Try To Set The Pixel Format
	{
		// Failed
		ReleaseDC (window.hWnd, window.hDC);							// Release Our Device Context
		window.hDC = 0;												// Zero The Device Context
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	window.hRC = wglCreateContext (window.hDC);						// Try To Get A Rendering Context
	if (window.hRC == 0)												// Did We Get A Rendering Context?
	{
		// Failed
		ReleaseDC (window.hWnd, window.hDC);							// Release Our Device Context
		window.hDC = 0;												// Zero The Device Context
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	// Make The Rendering Context Our Current Rendering Context
	if (wglMakeCurrent (window.hDC, window.hRC) == FALSE)
	{
		// Failed
		wglDeleteContext (window.hRC);									// Delete The Rendering Context
		window.hRC = 0;												// Zero The Rendering Context
		ReleaseDC (window.hWnd, window.hDC);							// Release Our Device Context
		window.hDC = 0;												// Zero The Device Context
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	ShowWindow (window.hWnd, SW_NORMAL);								// Make The Window Visible
	window.isVisible = TRUE;											// Set isVisible To True

	ReshapeGL (window.init.width, window.init.height);				// Reshape Our GL Window

	window.lastTickCount = GetTickCount ();							// Get Tick Count

	return TRUE;														// Window Creating Was A Success
																		// Initialization Will Be Done In WM_CREATE
}

BOOL DestroyWindowGL ()								// Destroy The OpenGL Window & Release Resources
{
	if (window.hWnd != 0)												// Does The Window Have A Handle?
	{	
		if (window.hDC != 0)											// Does The Window Have A Device Context?
		{
			wglMakeCurrent (window.hDC, 0);							// Set The Current Active Rendering Context To Zero
			if (window.hRC != 0)										// Does The Window Have A Rendering Context?
			{
				wglDeleteContext (window.hRC);							// Release The Rendering Context
				window.hRC = 0;										// Zero The Rendering Context

			}
			ReleaseDC (window.hWnd, window.hDC);						// Release The Device Context
			window.hDC = 0;											// Zero The Device Context
		}
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
	}

	if (window.init.isFullScreen)										// Is Window In Fullscreen Mode
	{
		ChangeDisplaySettings (NULL,0);									// Switch Back To Desktop Resolution
	}
	ShowCursor(TRUE);
	return TRUE;														// Return True
}

// Process Window Message Callbacks
LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Get The Window Context
	GL_Window* window = (GL_Window*)(GetWindowLong (hWnd, GWL_USERDATA));

	switch (uMsg)														// Evaluate Window Message
	{
		case WM_SYSCOMMAND:												// Intercept System Commands
		{
			switch (wParam)												// Check System Calls
			{
				case SC_SCREENSAVE:										// Screensaver Trying To Start?
				case SC_MONITORPOWER:									// Monitor Trying To Enter Powersave?
				return 0;												// Prevent From Happening
			}
			break;														// Exit
		}
		return 0;														// Return

		case WM_CREATE:													// Window Creation
		{
			CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);			// Store Window Structure Pointer
			window = (GL_Window*)(creation->lpCreateParams);
			SetWindowLong (hWnd, GWL_USERDATA, (LONG)(window));
		}
		return 0;														// Return

		case WM_CLOSE:													// Closing The Window
			TerminateApplication();								// Terminate The Application
		return 0;														// Return

		case WM_SIZE:													// Size Action Has Taken Place
			switch (wParam)												// Evaluate Size Action
			{
				case SIZE_MINIMIZED:									// Was Window Minimized?
					window->isVisible = FALSE;							// Set isVisible To False
				return 0;												// Return

				case SIZE_MAXIMIZED:									// Was Window Maximized?
					window->isVisible = TRUE;							// Set isVisible To True
					ReshapeGL (LOWORD (lParam), HIWORD (lParam));		// Reshape Window - LoWord=Width, HiWord=Height
				return 0;												// Return

				case SIZE_RESTORED:										// Was Window Restored?
					window->isVisible = TRUE;							// Set isVisible To True
					ReshapeGL (LOWORD (lParam), HIWORD (lParam));		// Reshape Window - LoWord=Width, HiWord=Height
				return 0;												// Return
			}
		break;															// Break

		case WM_KEYDOWN:												// Update Keyboard Buffers For Pressed
			keys[wParam] = TRUE;					// Set The Selected Key (wParam) To True
			
		break;															// Break

		case WM_KEYUP:													// Update Keyboard Buffers For Keys Released
			keys[wParam] = FALSE;					// Set The Selected Key (wParam) To False
			
		break;															// Break

		case WM_TOGGLEFULLSCREEN:										// Toggle FullScreen Mode On/Off
			g_createFullScreen = (g_createFullScreen == TRUE) ? FALSE : TRUE;
			PostMessage (hWnd, WM_QUIT, 0, 0);
		break;															// Break

		case WM_MOUSEWHEEL:
			newMouseX = wParam;
				if(newMouseX>0)
					camera->zoom(-1);
				else
					camera->zoom(1);
			break;

		case WM_RBUTTONDOWN:
			
				mouse_x = LOWORD(lParam);          
				mouse_y = HIWORD(lParam);
				rMouseSelection();
				if(glRender->clickWait == 0)
					glRender->clickWait = glRender->clickWaitTotal;

		break;

		case WM_MBUTTONDOWN:
				if(selectedID != -1)
					camera->setFocus(*actorManager->getActorByID(selectedID));
			
			break;

		case WM_LBUTTONDOWN:
			
	            mouse_x = LOWORD(lParam);          
				mouse_y = HIWORD(lParam);
				lMouseSelection();
			
		break;

		case WM_MOUSEMOVE:
			
	            mouse_x = LOWORD(lParam);          
				mouse_y = HIWORD(lParam);
		break;
	}

	return DefWindowProc (hWnd, uMsg, wParam, lParam);					// Pass Unhandled Messages To DefWindowProc
}

void lMouseSelection()
{
	double mvmatrix[16];
	double projmatrix[16];
	int viewport[4];

	double dX=0, dY=0, dZ=0, dClickY;
   
	//Get data from GL viewing matricies
	glGetIntegerv(GL_VIEWPORT, viewport);	
	glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
	dClickY = double(screenH - mouse_y); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	// This function will find 2 points in world space that are on the line into the screen defined by screen-space( ie. window-space ) point (x,y)
	gluUnProject ((double) mouse_x, dClickY, 0.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
	setVector(clickRay1, dX, dY, dZ);
   
	gluUnProject ((double) mouse_x, dClickY, 1.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
	setVector(clickRay2, dX, dY, dZ);

	clearCurrentSelection();

	if(actorManager->checkHitBoxes(selectedID, clickRay1, clickRay2))
	{
		setVector(glRender->target, NULL,NULL,NULL);
		Actor *actor = actorManager->getActorByID(selectedID);
		if(!gameManager->isSelectable(actor))
			clearCurrentSelection();
	}
}

void rMouseSelection()
{
	double mvmatrix[16];
	double projmatrix[16];
	int viewport[4];

	double dX=0, dY=0, dZ=0, dClickY;
   
	//Get data from GL viewing matricies
	glGetIntegerv(GL_VIEWPORT, viewport);	
	glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
	dClickY = double(screenH - mouse_y); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	// This function will find 2 points in world space that are on the line into the screen defined by screen-space( ie. window-space ) point (x,y)
	gluUnProject ((double) mouse_x, dClickY, 0.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
	setVector(clickRay1, dX, dY, dZ);
   
	gluUnProject ((double) mouse_x, dClickY, 1.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
	setVector(clickRay2, dX, dY, dZ);

	//This will later be changed to check all object/actors to check for hits
	//if(testBox.checkHitBox(clickRay1, clickRay2))
	//{
	//	setVector(target, NULL,NULL,NULL);
	//	mouseMode = 1;
	//	return;
	//}

	Vector2 start, end;
	Vector3 target;
	//If initial line points are reversed then the click hit will be last in the calculation
	BOOL reverse = FALSE, steep = FALSE, xReverse = FALSE, yReverse = FALSE;
	double dErr = 0, err = 0;
	int yStep = 0, x, y;

	//Set up starting points for Bresenham's
	if(abs(clickRay2.z - clickRay1.z) > abs(clickRay2.x - clickRay1.x))
	{
		//Rotate around the line x = y
		setCoord(start, clickRay1.z, clickRay1.x);
		setCoord(end, clickRay2.z, clickRay2.x);
		steep = TRUE;
	}
	else
	{
		setCoord(start, clickRay1.x, clickRay1.z);
		setCoord(end, clickRay2.x, clickRay2.z);
	}
	
	if(start.x > end.x)
	{
		double temp;
		temp = start.x;
		start.x = end.x;
		end.x = temp;
		
		temp = start.y;
		start.y = end.y;
		end.y = temp;
		reverse = TRUE;
	}

	//If both are true than that is the same situation as both being false
	//if(reverseX && reverseY)			
		//reverseX = reverseY = FALSE;

	dX = end.x - start.x;
	dZ = abs(end.y - start.y);
	dErr = dZ/dX;
	y = start.y;

	if(start.y < end.y)
		yStep = 1;
	else
		yStep = -1;

	for(x = start.x; x <= end.x; x++)
	{
		if(x < 0 || y < 0 || x > MAX_WORLD_SIZE-1 || y > MAX_WORLD_SIZE-1)
		{
			err += dErr;
			if(err >= .5)
			{
				y += yStep;
				err -= 1;
			}
			continue;
		}

		Vector3 tp1, tp2, tp3, tp4;
		
		if(steep)
		{
			for(int step = -1; step < 2; step++)
			{
				if(y+step < 0 || y+step > MAX_WORLD_SIZE-1)
					continue;

				setVector(tp1, y+step, terrainManager->root->world->getTerrainHeight(y+step, x), x);
				setVector(tp2, y+step+1, terrainManager->root->world->getTerrainHeight(y+step+1, x), x);
				setVector(tp3, y+step+1, terrainManager->root->world->getTerrainHeight(y+step+1, x+1), x+1);
				setVector(tp4, y+step, terrainManager->root->world->getTerrainHeight(y+step, x+1), x+1);


				//Did the click ray pass through either triangle in the chunk plane?
				if(checkLineIntersect(tp2, tp1, tp3, clickRay1, clickRay2, target) || checkLineIntersect(tp4, tp3, tp1, clickRay1, clickRay2, target))
				{
					if(reverse)
						continue;
					else
					{
						if(selectedID != -1)
							//actorManager->getActorByID(selectedID)->setMoveTarget(glRender->target);
							terrainManager->setMovePath(actorManager->getActorByID(selectedID), target);
							//actorManager->setMovePath(selectedID, glRender->target);
						setVector(glRender->target, target.x, target.y, target.z);
						return;		//Target found
					}
				}
			}

		}
		else
		{
			for(int step = -1; step < 2; step++)
			{
				if(x+step < 0 || x+step > MAX_WORLD_SIZE-1)
					continue;

				setVector(tp1, x, terrainManager->root->world->getTerrainHeight(x, y+step), y+step);
				setVector(tp2, x + 1, terrainManager->root->world->getTerrainHeight(x+1, y+step), y+step);
				setVector(tp3, x + 1, terrainManager->root->world->getTerrainHeight(x+1, y+step+1), y+step+1);
				setVector(tp4, x, terrainManager->root->world->getTerrainHeight(x, y+step+1), y+step+1);

			
				if(checkLineIntersect(tp2, tp1, tp3, clickRay1, clickRay2, target) || checkLineIntersect(tp4, tp3, tp1, clickRay1, clickRay2, target))
				{
					if(reverse)
						continue;
					else
					{
						if(selectedID != -1 && glRender->clickWait == 0)
							//actorManager->getActorByID(selectedID)->setMoveTarget(glRender->target);
							//finalTarget = &(glRender->target);
							terrainManager->setMovePath(actorManager->getActorByID(selectedID), target);
						setVector(glRender->target, target.x, target.y, target.z);
						return;		//Target found
					}
				}
			}
		}

		err += dErr;
		if(err >= .5)
		{
			y += yStep;
			err -= 1;
		}
	}

	setVector(glRender->target, target.x, target.y, target.z);

	if(selectedID != -1 && glRender->clickWait == 0)
		terrainManager->setMovePath(actorManager->getActorByID(selectedID), target);
}



//
//  FUNCTION: buildFont()
//
//  PURPOSE: Initializes OpenGL settings to make display fonts work
//
//  COMMENTS:
//


void buildFont(GLuint &fontSet, LPCWSTR fontName, int size)
{
	HFONT font;
	HFONT oldfont;

	fontSet = glGenLists(96);

	font = CreateFont(-size,
					  0,
					  0,
					  0,
					  FW_BOLD,
					  FALSE,
					  FALSE,
					  FALSE,
					  ANSI_CHARSET,
					  OUT_TT_PRECIS,
					  CLIP_DEFAULT_PRECIS,
					  NONANTIALIASED_QUALITY,
					  FF_DECORATIVE|DEFAULT_PITCH,
					  fontName);
	
	oldfont = (HFONT)SelectObject(window.hDC, font);
	wglUseFontBitmaps(window.hDC, 32, 96, fontSet);
}



