#include "LSFinterface.h"
// buffer to be used to store the hits during picking
#define BUFSIZE 256
GLuint selectBuf[BUFSIZE];


LSFinterface::LSFinterface(LSFscene *scene)
{
	this->scene = scene;

	face=GL_FRONT_AND_BACK;
	mode=GL_FILL;
}

LSFinterface::~LSFinterface(){}


void LSFinterface::initGUI()
{

	globals = scene->getGlobals();
	lights = scene->getLights();
	cameras = scene->getCameras();


	GLUI_Panel *lightsPanel = addPanel((char*)"Scenario", 1);
	addColumn();

	GLUI_Panel *polygonalModePanel = addPanel((char*)"Polygonal Mode", 1);
	addColumn();

	GLUI_Panel *gamePanel = addPanel((char*)"Game Moves", 1);
	addColumn();

	GLUI_Panel *rulesPanel = addPanel((char*)"Rules / Markers", 1);
	addColumn();

	// Scenario
	addButtonToPanel(lightsPanel,(char*)"Desert",10);
	addButtonToPanel(lightsPanel,(char*)"Jungle",11);
	addButtonToPanel(lightsPanel,(char*)"Village",12);

	// Game Panel
	addButtonToPanel(gamePanel,(char*)"Undo",20);
	addButtonToPanel(gamePanel,(char*)"Replay",21);

	//Game Rules
	addButtonToPanel(rulesPanel,(char*)"Show Rules",22);
	addButtonToPanel(rulesPanel,(char*)"Show Markers",23);

	int i;

	GLUI_RadioGroup* polygonalRadioGroup = addRadioGroupToPanel(polygonalModePanel, &polygonalMode, lights->size()+1);
	addRadioButtonToGroup(polygonalRadioGroup,(char*) "fill");
	addRadioButtonToGroup(polygonalRadioGroup, (char*)"lines");
	addRadioButtonToGroup(polygonalRadioGroup, (char*)"points");

	if(strcmp(globals->polygon_mode, "fill") == 0){
		mode = GL_FILL; polygonalMode = 0;
	}
	else if(strcmp(globals->polygon_mode, "line") == 0){
		mode = GL_LINE;  polygonalMode = 1;
	}
	else if(strcmp(globals->polygon_mode, "point") == 0){
		mode = GL_POINT;  polygonalMode = 2;
	}
}

void LSFinterface::processGUI(GLUI_Control *ctrl)
{
	cout << "ctrl =" << ctrl->user_id << endl;

	switch(ctrl->user_id){
		case 10:this->scene->scenario="SandScenario";  break;
		case 11:this->scene->scenario="JungleScenario"; break;
		case 12:this->scene->scenario="VillageScenario"; break;

		case 22:this->scene->setGameRules(true); break;
		case 23:this->scene->setGameRules(false); break;
	}

	map<string, LSFcamera*>::iterator itC;
	for(itC = cameras->begin(); itC != cameras->end(); itC++){
		if((*itC).second->cameraNum == camerasGroup){
			scene->activateCamera((*itC).first);
		}
	}

	if((unsigned int)ctrl->user_id == lights->size()+1)
		switch(polygonalMode){
		case 0: scene->setPolygonMode(face, mode=GL_FILL); break;
		case 1: scene->setPolygonMode(face, mode=GL_LINE); break;
		case 2: scene->setPolygonMode(face, mode=GL_POINT); break;
		}

}


/* From the example of picking */
void LSFinterface::processMouse(int button, int state, int x, int y) 
{
	CGFinterface::processMouse(button,state, x, y);

	// do picking on mouse press (GLUT_DOWN)
	// this could be more elaborate, e.g. only performing picking when there is a click (DOWN followed by UP) on the same place
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		performPicking(x,y);
		if(scene->isDemoModeStarted())
			this->scene->stopDemoMode();
	}
}

void LSFinterface::performPicking(int x, int y) 
{
	// Sets the buffer to be used for selection and activate selection mode
	glSelectBuffer (BUFSIZE, selectBuf);
	glRenderMode(GL_SELECT);

	// Initialize the picking name stack
	glInitNames();

	// The process of picking manipulates the projection matrix
	// so we will be activating, saving and manipulating it
	glMatrixMode(GL_PROJECTION);

	//store current projmatrix to restore easily in the end with a pop
	glPushMatrix ();

	//get the actual projection matrix values on an array of our own to multiply with pick matrix later
	GLfloat projmat[16];
	glGetFloatv(GL_PROJECTION_MATRIX,projmat);

	// reset projection matrix
	glLoadIdentity();

	// get current viewport and use it as reference for 
	// setting a small picking window of 5x5 pixels around mouse coordinates for picking
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// this is multiplied in the projection matrix
	gluPickMatrix ((GLdouble) x, (GLdouble) (CGFapplication::height - y), 5.0, 5.0, viewport);

	// multiply the projection matrix stored in our array to ensure same conditions as in normal render
	glMultMatrixf(projmat);

	// force scene drawing under this mode
	// only the names of objects that fall in the 5x5 window will actually be stored in the buffer
	this->scene->display();
	this->scene->selectionMode();

	// restore original projection matrix
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();

	glFlush();

	// revert to render mode, get the picking results and process them
	GLint hits;
	hits = glRenderMode(GL_RENDER);
	processHits(hits, selectBuf);
}

void LSFinterface::processHits (GLint hits, GLuint buffer[]) 
{
	GLuint *ptr = buffer;
	GLuint mindepth = 0xFFFFFFFF;
	GLuint *selected=NULL;
	GLuint nselected;

	// iterate over the list of hits, and choosing the one closer to the viewer (lower depth)
	for (int i=0;i<hits;i++) {
		int num = *ptr; ptr++;
		GLuint z1 = *ptr; ptr++;
		ptr++;
		if (z1 < mindepth && num>0) {
			mindepth = z1;
			selected = ptr;
			nselected=num;
		}
		for (int j=0; j < num; j++) 
			ptr++;
	}
	
	// if there were hits, the one selected is in "selected", and it consist of nselected "names" (integer ID's)
	if (selected!=NULL)
	{
		// this should be replaced by code handling the picked object's ID's (stored in "selected"), 
		// possibly invoking a method on the scene class and passing "selected" and "nselected"
		printf("Picked ID's: ");
		for (int i=0; i<nselected; i++){
			printf("%d ",selected[i]);
			if(selected[i]<12 && selected[i]>=0){
				this->scene->boardHandler(selected[i]);

			}
		}
		printf("\n");
	}
	else
		printf("Nothing selected while picking \n");	
}
