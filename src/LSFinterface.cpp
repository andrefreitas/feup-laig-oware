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


	GLUI_Panel *lightsPanel = addPanel((char*)"Lights", 1);
	addColumn();
	GLUI_Panel *camerasPanel = addPanel((char*)"Cameras", 1);
	addColumn();
	GLUI_Panel *polygonalModePanel = addPanel((char*)"Polygonal Mode", 1);
	addColumn();

	//para numerar os elementos da interface
	int i = 0;

	bool divided = false;
	map<string, LSFlight*>::iterator itL;
	for(itL = lights->begin(); itL != lights->end(); itL++, i++){
		addButtonToPanel(lightsPanel,(char*)(*itL).second->id.c_str(),i);
		(*itL).second->lightNum = i;
		if(i==3 && !divided){
			addColumnToPanel(lightsPanel);
			divided = true;
		}
		if(DEBUGMODE) cout << *(&(*itL).second->lightNum) << endl;
	}

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
	map<string, LSFlight*>::iterator itL;
	for(itL = lights->begin(); itL != lights->end(); itL++){
		cout << (*itL).second->lightNum << " - " << ctrl->user_id << endl;
		if((*itL).second->lightNum == ctrl->user_id){
			if((*itL).second->enabled)
				(*itL).second->enabled = false;
			else
				(*itL).second->enabled = true;
		}
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


