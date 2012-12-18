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
	// Default params
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glEnable ( GL_TEXTURE_2D );
	glNormal3f(0,0,1);
	defaultAppearance = new LSFappearance();
	defaultAppearance->appearance = new CGFappearance();
	float color[4]={0.6,0.6,0.6,0.6};
	defaultAppearance->appearance->setAmbient(color);
	defaultAppearance->appearance->setDiffuse(color);
	defaultAppearance->appearance->setSpecular(color);
	defaultAppearance->appearance->setShininess(0.5);

	// Parse configurations and other data
	sceneParser = new LSFparser(inputScene);
	setGlobals();
	sceneParser->getAppearances(appearances);
	sceneParser->getLights(lights,lights_enabled, lights_local, lights_doublesided,ambient);
	if(lights_enabled) glEnable(GL_LIGHTING);
	if(lights_doublesided) glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	if(lights_local) glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	sceneParser->getNodes(nodes, rootNode);
	sceneParser->getCameras(cameras);
	sceneParser->getAnimations(animations);
	LSFcamera *freeCamera = new LSFcamera;
	freeCamera->id = "freeMove";
	cameras["freeMove"] = freeCamera;

	stack<LSFappearance*> appearancesStack_temp;
	appearancesStack_temp.push(defaultAppearance);
	initPrimitives(nodes, rootNode, appearances, appearancesStack_temp);
	initCameras();
	setUpdatePeriod(10);
}

void LSFscene::initPrimitives(map<string,LSFnode*> &nodes,string &rootNode,map<string,LSFappearance*> appearances,
		                      stack<LSFappearance*> &appearancesStack){
	LSFappearance *currentAppearance;
	if (nodes[rootNode]->appearance == "inherit")
		currentAppearance = appearancesStack.top();
	else
		currentAppearance = appearances[nodes[rootNode]->appearance];

	appearancesStack.push(currentAppearance);

	for(unsigned int i = 0; i < nodes[rootNode]->childPrimitives.size(); i++)
		nodes[rootNode]->childPrimitives[i]->init(currentAppearance);

	for(unsigned int i = 0; i < nodes[rootNode]->childNoderefs.size(); i++)
		initPrimitives(nodes, nodes[rootNode]->childNoderefs[i], appearances, appearancesStack);
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

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPolygonMode(face, mode);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// ---- BEGIN cameras section
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
	// ---- END cameras section

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



	// ---- BEGIN Primitive drawing section

	axis.draw();

	stack<LSFappearance*> appearancesStack;
	appearancesStack.push(defaultAppearance);
	LSFrender::render(nodes,rootNode,appearances,appearancesStack,animations,LSFscene::timeSeconds); 

	// ---- END Primitive drawing section

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
