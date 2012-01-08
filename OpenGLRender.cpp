#include "OpenGLRender.h"


OpenGLRender::OpenGLRender(Camera &c, ActorManager &a, TerrainManager &t, GLuint &fSet, int &sID)
{
	fontSet = &fSet;
	aM = &a;
	tM = &t;
	active=TRUE;										// Flag sets window to be active by default
	fullscreen=TRUE;									// Flag sets window to be fullscreen by default
	light = true;

	cam = &c;

	//Debug display variables
	debug = FALSE;
	drawX=0;
	drawY=0;								//Location to draw text on the screen
	overlayLineCount=0;
	fpsUpdate = 0;
	selectedID = &sID;
	clickWait = 0;
	clickWaitTotal = 300;

	for(int i = 0; i < 256; i++)
		keys[i] = false;
}


BOOL OpenGLRender::initialize(Vector2* windowSize)
{
	screenH = windowSize->y;
	screenW = windowSize->x;

	//glShadeModel(GL_SMOOTH);
	glClearColor(0.3f,0.3f,0.7f,0.0f);
	glClearDepth(1.0f);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);	
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	//glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	
	GLfloat LightAmbient[]= { .3,.3, .3, 1.0f };
	GLfloat LightDiffuse[]= { 1.0f, 1, 1, 1.0f };
	GLfloat LightSpecular[]= { 1.0f, 1, 1, 1.0f };
	GLfloat LightPosition[]= { -1.0f,1.0f, 0.0f, 0.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	//glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, .05);
	glEnable(GL_LIGHT1);
	checkLighting(true);
	perspective();

	loadTextures();
	//drawTerrainAsList();


	return TRUE;
}


//
//  FUNCTION: DrawGLScene()
//
//  PURPOSE: Draw everything in the OpenGL environment
//
//  COMMENTS:
//

int OpenGLRender::drawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glLoadIdentity();

	if(light)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	checkLighting(true);
	//tM->drawTerrain();
	fastDrawTerrain();
	//drawTerrain();
	//glScalef(worldScale, 1.0, worldScale);
	//glCallList(terrainList);		//Draw Terrain List
	//glScalef(-worldScale, -1.0, -worldScale);

	glBindTexture( GL_TEXTURE_2D, 0);

	//Display the movement path that is stored by the selected object
	if(*selectedID != -1)
	{
		nodePath *next = aM->getActorByID(*selectedID)->getPathRoot();
		while(next != NULL)
		{
			glColor3f(1,1,0);
			glBegin(GL_LINES);
				glVertex3f(next->nodeData.x, tM->getHeight((next->nodeData.x), (next->nodeData.y)), next->nodeData.y);
				glVertex3f(next->nodeData.x+1, tM->getHeight((next->nodeData.x), (next->nodeData.y))+3, next->nodeData.y-1);
			glEnd();
			
			next = next->next;
		}
	}

	//Display the grid of movement nodes with their current incentive value
	/*for(int a = 0; a < MAX_WORLD_SIZE/32; a++)
		for(int b = 0; b < MAX_WORLD_SIZE/32; b++)
		{
			glColor3f(1-(float)manager->world->nodes[a][b].incentive/15.0,0,0);
			glBegin(GL_LINES);
	
					if(manager->world->nodes[a][b].incentive != -2)
					{
						Vector3 v, w;
						setVector(v, manager->world->nodes[a][b].nodeData.x, *manager->world->getTerrainHeight(a*32, b*32), manager->world->nodes[a][b].nodeData.y);
						setVector(w, manager->world->nodes[a][b].nodeData.x, *manager->world->getTerrainHeight(a*32, b*32)+5, manager->world->nodes[a][b].nodeData.y);
						
						glVertex3f(v.x, v.y, v.z);
						glVertex3f(w.x, w.y, w.z);
					}
			glEnd();

			if(manager->world->nodes[a][b].incentive > -1)
			{
				glColor3f(0,0,1-(float)manager->world->nodes[a][b].incentive/15.0);
				glTranslated(manager->world->nodes[a][b].nodeData.x, *manager->world->getTerrainHeight(a*32, b*32)+5, manager->world->nodes[a][b].nodeData.y);
				glRasterPos2f(0,0);
				string str = dtos(manager->world->nodes[a][b].incentive);
				const char *c = str.c_str();
				glPrint(c);
				glLoadIdentity();
			}
		}*/

	glLoadIdentity();
	
	//Display the location of the last right click on the terrain
	if(target.x != -1)
	{
		glColor3f(1,1,1);
		checkLighting(false);
		glBegin(GL_LINES);
			glVertex3f(target.x, target.y, target.z);
			glVertex3f(target.x, target.y+5, target.z);

			glVertex3f(target.x+1, target.y, target.z-1);
			glVertex3f(target.x-1, target.y, target.z+1);

			glVertex3f(target.x+1, target.y, target.z+1);
			glVertex3f(target.x-1, target.y, target.z-1);
		glEnd();
		

		//LightPosition[0] = target.x;
		//LightPosition[1] = target.y-100;
		//LightPosition[2] = target.z;
		//glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	}

	//testCube.draw();
	//glmDraw(testCube, GLM_SMOOTH| GLM_TEXTURE); 
	//---DEBUG DISPLAY

	checkLighting(true);
	aM->drawObjects();
	overlayDisplay();
	
	//------
	return TRUE;
}


//
//  FUNCTION: overlayDisplay()
//
//  PURPOSE: Display data to the screen using OpenGL fonts
//
//  COMMENTS: Currently being used to display variables at runtime for debugging purposes
//

void OpenGLRender::overlayDisplay()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		glOrtho(0, screenW, 0, screenH, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();

			//Lighting should not affect objects in the overlay
			checkLighting(false);
			
			//Only display debug text if user turned it on
			if(!debug)
			{
				//glTranslatef(cam.getPosX(),cam.getPosY()-1,cam.getPosZ()+1);
				//glTranslatef(0,0,0);
				glColor3f(1.0f,1.0f,1.0f);

				overlayLineCount = 0;
				rasterStringToOverlay("("+itos(cam->getLookAt().x)+","+ dtos(cam->getLookAt().y)+","+ itos(cam->getLookAt().z)+")");
				rasterStringToOverlay(itos(mouseX) + "," + itos(mouseY));
				if(*selectedID != -1)
				{
					rasterStringToOverlay("Box Data:");
					rasterStringToOverlay("     Box Height: "+itos(aM->getActorByID(*selectedID)->getLocation().y));
					rasterStringToOverlay("     Box ID: "+itos(*selectedID));
				}
				rasterStringToOverlay("FPS: " + dtos(1000.0/time));
			}

			//Draw the cursor
			int xTex = 25, yTex = 59;
			glBindTexture(GL_TEXTURE_2D, testTex[cursorTex]);
			glBegin(GL_QUADS);
				glTexCoord2f(1,1); glVertex3f(mouseX+xTex, screenH-mouseY,0);
				glTexCoord2f(0,1); glVertex3f(mouseX, screenH-mouseY,0);
				glTexCoord2f(0,0); glVertex3f(mouseX, screenH-mouseY-yTex,0);
				glTexCoord2f(1,0); glVertex3f(mouseX+xTex, screenH-mouseY-yTex,0);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);

			checkLighting(true);
		glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glPopMatrix();


	
	perspective();
}


//Utility function to make displaying lines of text on the overlay less messy
void OpenGLRender::rasterStringToOverlay(string str)
{
	glRasterPos2f(1,screenH-16-(overlayLineCount*16));
	const char *c = str.c_str();//new char(str.size()+1);
	//strcpy (c, str.c_str());
	glPrint(c);
	overlayLineCount++;
}

void OpenGLRender::fastDrawTerrain()
{
	int i = 0;
	bool restart = true;
	//int z = 0;
	for(int z = 0; z < MAX_WORLD_SIZE-1; z++)
	{
		restart = true;
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < MAX_WORLD_SIZE-1; x++)
		{
			double h1 = tM->getHeight(x,z);
			double h2 = tM->getHeight(x+1,z);
			double h3 = tM->getHeight(x+1,z+1);
			double h4 = tM->getHeight(x,z+1);

			Vector3 n1, n2, normal1, normal2;
			if(tM->getType(x,z) != i)
			{
				glEnd();

				i = tM->getType(x,z);
				glBindTexture( GL_TEXTURE_2D, testTex[i]);
				restart = true;
				glBegin(GL_TRIANGLE_STRIP);
			}

			//Get normal vector for the first triangle
			setVector(n1, 0, h3-h2, 1);
			setVector(n2, -1, h1-h2, 0);
			normal1 = crossProduct(n2, n1);
			normalize(normal1);

			//Get normal vector for the second triangle
			setVector(n1, 1, h3-h4, 0);
			setVector(n2, 0, h1-h4, -1);
			normal2 = crossProduct(n1, n2);
			normalize(normal2);

			float xTex = .1, zTex = .1, texStep;
			int texSize = 128;

			texStep = 1.0/texSize;

			xTex = (float)(x % texSize)/texSize;
			zTex = (float)(z % texSize)/texSize;
			if(x%texSize == 0 || z%texSize == 0)
				restart = true;

			//glBegin(GL_TRIANGLE_STRIP);
			if(restart)
			{
				glNormal3d(normal1.x, normal1.y, normal1.z);
				glTexCoord2f(xTex, zTex);						glVertex3f(x,	h1,z);
				glTexCoord2f(xTex, zTex + texStep);				glVertex3f(x,	h4,(z+1));
				glTexCoord2f(xTex + texStep, zTex);				glVertex3f((x+1),	h2,z);
					
				glNormal3d(normal2.x, normal2.y, normal2.z);
					
				glTexCoord2f(xTex + texStep, zTex + texStep);	glVertex3f((x+1), h3,(z+1));
				restart = false;
			}
			else
			{
				glNormal3d(normal1.x, normal1.y, normal1.z);
				glTexCoord2f(xTex + texStep, zTex);				glVertex3f((x+1),	h2,z);

				glNormal3d(normal2.x, normal2.y, normal2.z);
				glTexCoord2f(xTex + texStep, zTex + texStep);	glVertex3f((x+1), h3,(z+1));
			}
			//glEnd();
		}
		glEnd();
	}
}


void OpenGLRender::drawTerrain()
{
	//terrainList = glGenLists(1);
	//glNewList(terrainList, GL_COMPILE);

	//Find all the tiles that use the first texture and then loop to the next texture
	for(int i = 0; i < 5; i++)
	{
		glBindTexture( GL_TEXTURE_2D, testTex[i]);

		//Loop over all terrain tiles
		for(int z = 0; z < MAX_WORLD_SIZE-1; z++)
			for(int x = 0; x < MAX_WORLD_SIZE-1; x++)
			{
				//Is the current tile using the loaded texture?
				if(tM->getType(x,z) != i)
					continue;
				
				double h1 = tM->getHeight(x,z);
				double h2 = tM->getHeight(x+1,z);
				double h3 = tM->getHeight(x+1,z+1);
				double h4 = tM->getHeight(x,z+1);

				Vector3 n1, n2, normal1, normal2;

				//Get normal vector for the first triangle
				setVector(n1, 0, h3-h2, 1);
				setVector(n2, -1, h1-h2, 0);
				normal1 = crossProduct(n2, n1);
				normalize(normal1);

				//Get normal vector for the second triangle
				setVector(n1, 1, h3-h4, 0);
				setVector(n2, 0, h1-h4, -1);
				normal2 = crossProduct(n1, n2);
				normalize(normal2);

				float xTex = .1, zTex = .1, texStep;

				texStep = 1/8.0;

				xTex = (float)(x % 8)/8.3;
				zTex = (float)(z % 8)/8.3;
				
				/*Vector3 color;
				switch(world.getTerrainObject(x, z))
				{
				case 0:		//Draw Rock
					setVector(color, .2, .2, .2);
					break;

				case 1:		//Draw bush
					setVector(color, .1, .4, .1);
					break;

				case 2:		//Draw Tree
					setVector(color, .5, .5, .1);
					break;

				case 3:		//Draw cactus
					setVector(color, .1, .4, .1);
					break;
				
				default:
					setVector(color, 1,1,1);
					break;
				}*/
			
				
				/*glBegin(GL_QUADS);
				glNormal3d(normal1.x, normal1.y, normal1.z);
				
					//world.getColor(x, z);
					glTexCoord2f(0.1f, 0.1f);	glVertex3f(x,	h1,z);

					//world.getColor(x+1, z);
					glTexCoord2f(.9f, 0.1f);	glVertex3f(x+1,	h2,z);

					//world.getColor(x+1, z+1);
					glTexCoord2f(.9f, .9f);	glVertex3f(x+1, h3,z+1);

					//world.getColor(x, z+1);
					glTexCoord2f(0.1f, .9f);	glVertex3f(x,	h4,z+1);
				glEnd();*/

				/*glBegin(GL_LINES);
					glVertex3f(x,	h1,z);
					glVertex3f(x+1, h2,z);
					glVertex3f(x,	h1,z);
					glVertex3f(x,	h4,z+1);
				glEnd();*/
				
				//if(world.getTerrainGradient(x,z) > .5)
					//glColor3f(0.0, 1, 0.0);
				glBegin(GL_TRIANGLES);
					
					glNormal3d(normal1.x, normal1.y, normal1.z);
					glTexCoord2f(xTex, zTex);						glVertex3f(x,	h1,z);
					glTexCoord2f(xTex + texStep, zTex + texStep);	glVertex3f(x+1, h3,z+1);
					glTexCoord2f(xTex + texStep, zTex);				glVertex3f(x+1,	h2,z);
					
					glNormal3d(normal2.x, normal2.y, normal2.z);
					
					glTexCoord2f(xTex, zTex);						glVertex3f(x,	h1,z);
					glTexCoord2f(xTex, zTex + texStep);				glVertex3f(x,	h4,z+1);
					glTexCoord2f(xTex + texStep, zTex + texStep);	glVertex3f(x+1, h3,z+1);
					
				glEnd();
			}
	}
	//glEndList();

	glBindTexture( GL_TEXTURE_2D, 0);
}






BOOL OpenGLRender::update(DWORD milliseconds, int mX, int mY)
{
	mouseX = mX;
	mouseY = mY;

	if(fpsUpdate > 250)
	{
		time = milliseconds;
		fpsUpdate -= 250;
	}
	fpsUpdate += milliseconds;
	
	clickWait -= milliseconds;
	if(clickWait < 0)
		clickWait = 0;

	if(cam->hasFocus())
		cam->followFocus();

	if(keys[VK_ESCAPE])
		return FALSE;

	//Toggle lighting if key is pressed
	toggleLighting(keys['L']);

	//Toggle debug display if key is pressed
	toggleDebug(keys['P']);

	//Moves the camera around
	if(keys['W'])
		cam->strafe(1,0.0, milliseconds);
	if(keys['S'])
		cam->strafe(-1,0.0, milliseconds);
	if(keys['A'])
		cam->strafe(0.0,1, milliseconds);
	if(keys['D'])
		cam->strafe(0.0,-1, milliseconds);

	//Zooms the camera in or out
	if(keys['Q'])
		cam->rotate(-1, milliseconds);
	if(keys['E'])
		cam->rotate(1, milliseconds);

	if(keys[VK_SPACE])
		aM->shoot(*selectedID);

	//if (g_keys->keyDown[VK_F1])									// Is F1 Being Pressed?
		//ToggleFullscreen (g_window);							// Toggle Fullscreen Mode

	//Adjust GL camera with the new frame
	perspective();
	return TRUE;
}



void OpenGLRender::checkLighting(bool lightSwitch)
{
	if(light)
	{
		if(lightSwitch)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
	}
	else
		glDisable(GL_LIGHTING);

}

//
//  FUNCTION: perspective()
//
//  PURPOSE: Changes the viewing angle for the camera when the 
//			 camera variables change
//
//  COMMENTS:
//

void OpenGLRender::perspective()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)screenW/(GLfloat)screenH,2.0f,1000.0f);

	double adjust = 0;
	if(cam->getPosition().y < tM->getHeight(cam->getPosition().x, cam->getPosition().z)+5)
		cam->adjustHeight(tM->getHeight(cam->getPosition().x, cam->getPosition().z)+5);

	gluLookAt(cam->getPosition().x,cam->getPosition().y,cam->getPosition().z,  
			  cam->getLookAt().x,cam->getLookAt().y,cam->getLookAt().z,  0,1,0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void OpenGLRender::toggleLighting(bool lighting)
{
	if(lighting && !lp)
	{
		lp = TRUE;
		light = !light;
	}
	if(!lighting)
		lp = FALSE;
}

void OpenGLRender::toggleDebug(bool debug)
{
	//Toggle the debug printing to the screen
	//Toggles at every key press
	if(debug && !debugP)
	{
		debugP = TRUE;
		debug = !debug;
	}
	if(!debug)
		debugP = FALSE;
}


//
//  FUNCTION: loadTexture()
//
//  PURPOSE: Loads a texture from the given file into the given array index
//
//  COMMENTS:
//

void OpenGLRender::loadTexture(string path, int index)
{
	testTex[index] = SOIL_load_OGL_texture
	(
		path.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	if(testTex[index])
		log("Texture "+path+" loaded successfully\n");
}

void OpenGLRender::loadTextures()
{
	loadTexture("data/images/grass.jpg", 0);
	loadTexture("data/images/sand_texture.jpg", 2);
	loadTexture("data/images/forest.jpg", 1);
	loadTexture("data/images/snow_texture.jpg", 3);
	loadTexture("data/images/water.jpg", 4);
	loadTexture("data/images/cursor2.png", 5);
	loadTexture("data/images/dirt_splash1.bmp", 6);
	cursorTex = 5;
}

//
//  FUNCTION: glPrint()
//
//  PURPOSE: Prints text to the screen using OpenGL fonts
//
//  COMMENTS:
//
void OpenGLRender::glPrint(const char *fmt)
{
	char text[256];
	va_list ap;

	if(fmt == NULL)
		return;

	va_start(ap, fmt);
		vsprintf_s(text, fmt, ap);
	va_end(ap);

	glListBase(*fontSet - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

OpenGLRender::~OpenGLRender(void)
{
}
