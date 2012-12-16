#include "LSFscene.h"

using namespace std;
float pi = acos(-1.0);
float deg2rad=pi/180.0;

CGFappearance *mat1;

LSFscene::LSFscene(){}
double LSFscene::timeSeconds=0;

LSFscene::LSFscene(char* argv[]){
	if(argv[1] == NULL) inputScene=(char*)"../lsf/default.lsf";
	else inputScene = argv[1];
}

LSFscene::~LSFscene(){

}

void LSFscene::init()
{
	// By default is false
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

	// Enables textures
	glEnable ( GL_TEXTURE_2D );

	// Sets up some lighting parameters
	 // Define ambient light

	// Defines a default normal
	glNormal3f(0,0,1);

	// Initializate the parser
	sceneParser = new LSFparser(inputScene);

	// Set the default appearance
	defaultAppearance = new LSFappearance();
	defaultAppearance->appearance = new CGFappearance();
	float color[4]={0.6,0.6,0.6,0.6};
	defaultAppearance->appearance->setAmbient(color);
	defaultAppearance->appearance->setDiffuse(color);
	defaultAppearance->appearance->setSpecular(color);
	defaultAppearance->appearance->setShininess(0.5);


	// Get Global configurations
	setGlobals();
	// Get the Appearances
	sceneParser->getAppearances(appearances);
	// Get the lights
	sceneParser->getLights(lights,lights_enabled, lights_local, lights_doublesided,ambient);
	// Lights config
	if(lights_enabled) glEnable(GL_LIGHTING);
	if(lights_doublesided) glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	if(lights_local) glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	// Get the nodes
	sceneParser->getNodes(nodes, rootNode);
	// Get the cameras info
	sceneParser->getCameras(cameras);
	sceneParser->getAnimations(animations);
	LSFcamera *freeCamera = new LSFcamera;
	freeCamera->id = "freeMove";
	cameras["freeMove"] = freeCamera;


	LSFappearance *currentAppearance;
	stack<LSFappearance*> appearancesStack_temp;
	appearancesStack_temp.push(defaultAppearance);
	for(map<string,LSFnode*>::iterator it = nodes.begin(); it != nodes.end(); it++){
		for(unsigned int i = 0; i < it->second->childPrimitives.size(); i++){
			if (it->second->appearance == "inherit")
				currentAppearance = appearancesStack_temp.top();
			else
				currentAppearance = appearances[it->second->appearance];

			appearancesStack_temp.push(currentAppearance);
			it->second->childPrimitives[i]->init(currentAppearance);
		}
	}

	initCameras();

	setUpdatePeriod(10);
}

map<string, LSFlight*> * LSFscene::getLights(){
	return &lights;
}

map<string, LSFcamera*> * LSFscene::getCameras(){
	return &cameras;
}

struct globalsData * LSFscene::getGlobals(){
	return &globals;
}

void LSFscene::initCameras()
{
	map<string,LSFcamera*>::iterator it;
	for(it = cameras.begin(); it != cameras.end(); it++){
		(*it).second->camera = new CGFcamera();
		if((*it).second->view == "perspective"){
			(*it).second->setPosition((*it).second->fromX, (*it).second->fromY, (*it).second->fromZ);
			(*it).second->setTarget((*it).second->toX, (*it).second->toY, (*it).second->toZ);
			(*it).second->setStartRotation();
		}
		else if((*it).second->view == "ortho"){
			(*it).second->setPosition((*it).second->left, (*it).second->top, (*it).second->_far);
			(*it).second->setTarget((*it).second->right, (*it).second->bottom, (*it).second->_near);
			(*it).second->setStartRotation();
		}
	}


	it = cameras.begin();
	activeCamera = (*it).second->id;
	if(DEBUGMODE) cout << activeCamera << endl;
}

void LSFscene::positionView(string activeCam, int axis, float newPosition){
	if (axis==0 || axis==1 || axis==2)
		cameras[activeCam]->camera->translate(axis, newPosition);
}

void LSFscene::activateCamera(string id)
{
	activeCamera = cameras[id]->id;
	if(DEBUGMODE) cout << "activeCamera = " << id << endl;
}

void LSFscene::display()
{

	// ---- BEGIN Background, camera and axis setup
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	// Clear image and depth buffer everytime we update the scene
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Initialize Model-View matrix as identity (no transformation

	//polygonal mode
	glPolygonMode(face, mode);

	// Apply transformations corresponding to the camera position relative to the origin

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//cameras aplyView
	if(activeCamera == "freeMove")
	{
		CGFscene::activeCamera->updateProjectionMatrix(CGFapplication::width, CGFapplication::height);
		glMatrixMode(GL_MODELVIEW);
		CGFscene::activeCamera->applyView();
	}
	else{
		cameras[activeCamera]->updateProjectionMatrix(CGFapplication::width, CGFapplication::height);
		cameras[activeCamera]->applyView();
	}



	// ---- BEGIN lights section
	map<string,LSFlight*>::iterator it;
	for (it=lights.begin();it!=lights.end(); it++){
		if((*it).second->enabled){
			(*it).second->light->enable();
			(*it).second->light->draw();
		}
		else{
			(*it).second->light->disable();
		}
	}

	// ---- END lights section

	// Draw axis
	axis.draw();

	// ---- END Background, camera and axis setup
	stack<LSFappearance*> appearancesStack;
	appearancesStack.push(defaultAppearance);
	LSFrender::render(nodes,rootNode,appearances,appearancesStack,animations,LSFscene::timeSeconds);


	// ---- BEGIN Primitive drawing section
	// ---- END Primitive drawing section

	// We have been drawing in a memory area that is not visible - the back buffer, 
	// while the graphics card is showing the contents of another buffer - the front buffer
	// glutSwapBuffers() will swap pointers so that the back buffer becomes the front buffer and vice-versa
	glutSwapBuffers();
}

void LSFscene::setPolygonMode(unsigned int face, unsigned int mode){
	this->face = face;
	this->mode = mode;
}

void LSFscene::setGlobals(){
	sceneParser->getGlobals(&globals);

	// Set Background
	glClearColor(globals.background[0], globals.background[1], globals.background[2], globals.background[3]);

	// Set polygon mode and shading
	GLenum  face=GL_FRONT_AND_BACK, mode=GL_FILL;
	if(strcmp(globals.polygon_mode,"line")==0) mode=GL_LINE;
	else if (strcmp(globals.polygon_mode,"point")==0) mode=GL_POINT;
	glPolygonMode(face, mode);

	// Set cullings params
	if(strcmp(globals.culling__frontfaceorder,"CW")==0) glFrontFace(GL_CW);
	face=GL_BACK;
	if(strcmp(globals.culling_cullface,"front")==0) face=GL_FRONT;
	if(strcmp(globals.culling_cullface,"both")==0) face=GL_FRONT_AND_BACK;
	glCullFace(face);
	if(globals.culling_enabled) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

}

void LSFscene::update(long millis){
	LSFscene::timeSeconds=(millis/1000.0);
}
