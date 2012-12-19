#include "LSFinterface.h"

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
	GLUI_Panel *camerasPanel = addPanel((char*)"Cameras", 1);
	addColumn();
	GLUI_Panel *polygonalModePanel = addPanel((char*)"Polygonal Mode", 1);
	addColumn();

	// Scenario
	addButtonToPanel(lightsPanel,"Desert",10);
	addButtonToPanel(lightsPanel,"Jungle",11);
	addButtonToPanel(lightsPanel,"Village",12);
	
	int i;

	GLUI_RadioGroup* radioGroup = addRadioGroupToPanel(camerasPanel, &camerasGroup, lights->size());
	map<string, LSFcamera*>::iterator itC;
	for(itC = cameras->begin(), i = 0; itC != cameras->end(); itC++, i++){
		addRadioButtonToGroup(radioGroup, (char*)(*itC).second->id.c_str());
		(*itC).second->cameraNum = i;
		if((*itC).second->initial){
			initialCamera = i;
			scene->activateCamera((*itC).first);
		}
		if(DEBUGMODE) cout << *(&(*itC).second->cameraNum) << endl;
	}
	camerasGroup = initialCamera;

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
		case 12:;this->scene->scenario="VillageScenario"; break;
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


