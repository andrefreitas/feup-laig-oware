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

	initCameras();
	setUpdatePeriod(10);

	// Scenario
	scenario="JungleScenario";

	timer = new Timer();

	//Players and startGame
	s1 = new Socket("127.0.0.1",6300);

	game = new Oware(new Computer("ABC", "bot2"), new Human("Paulo", "human"), 1);

	cout << game->getRoules() << endl;

	game->startServer(s1);
	game->startGame(s1, game->getPlayer1()->getType(), game->getPlayer2()->getType());
	//game->startGame(s1, player1->getType(), player2->getType(), "1", "[[1,2,3,4,5,6],[1,15,1,1,1,1]]", "0", "6");
	//game->startGame(s1, player1->getType(), player2->getType(), "1", "[[0,0,0,0,0,0],[0,0,0,0,0,0]]", "24", "24");
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

	//axis.draw();

	// Scenario
	stack<LSFappearance*> appearancesStack;
	appearancesStack.push(defaultAppearance);
	LSFrender::render(nodes,scenario,appearances,appearancesStack,animations,LSFscene::timeSeconds); 

	// Board
	stack<LSFappearance*> appearancesStack2;
	appearancesStack2.push(defaultAppearance);
	string board="Board";
	LSFrender::render(nodes,board,appearances,appearancesStack2,animations,LSFscene::timeSeconds);

	// Markers
	glPushMatrix();
	string markers="Markers";
	stack<LSFappearance*> appearancesStack3;
	appearancesStack3.push(defaultAppearance);
	LSFrender::render(nodes,markers,appearances,appearancesStack3,animations,LSFscene::timeSeconds);
	glPopMatrix();

	//Players seeds and timer

	string numbers;
	stack<LSFappearance*> appearancesStack4;
	appearancesStack4.push(defaultAppearance);
	//Player1 seeds
	if(game->getPlayer1()->getScore() < 10){
		numbers = numberToText(game->getPlayer1()->getScore());
		glPushMatrix();
			glTranslated(6, 11, 0);
			LSFrender::render(nodes,numbers,appearances,appearancesStack4,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}
	else{
		int score = game->getPlayer1()->getScore();
		numbers = numberToText(score/10);
		glPushMatrix();
			glTranslated(5, 11, 0);
			LSFrender::render(nodes,numbers,appearances,appearancesStack4,animations,LSFscene::timeSeconds);
		glPopMatrix();
		numbers = numberToText(score%10);
		glPushMatrix();
			glTranslated(7, 11, 0);
			LSFrender::render(nodes,numbers,appearances,appearancesStack4,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}

	stack<LSFappearance*> appearancesStack5;
	appearancesStack5.push(defaultAppearance);
	//Player2 seeds
	if(game->getPlayer2()->getScore() < 10){
		numbers = numberToText(game->getPlayer2()->getScore());
		glPushMatrix();
			glTranslated(6, 2, 0);
			LSFrender::render(nodes,numbers,appearances,appearancesStack5,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}
	else{
		int score = game->getPlayer2()->getScore();
		numbers = numberToText(score/10);
		glPushMatrix();
			glTranslated(5, 2, 0);
			LSFrender::render(nodes,numbers,appearances,appearancesStack5,animations,LSFscene::timeSeconds);
		glPopMatrix();
		numbers = numberToText(score%10);
		glPushMatrix();
			glTranslated(7, 2, 0);
			LSFrender::render(nodes,numbers,appearances,appearancesStack5,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}

	stack<LSFappearance*> appearancesStack6;
	appearancesStack6.push(defaultAppearance);
	//Timer
	if(!timer->isStarted())
		timer->startCountDown(game->getMaxTime());

	int remainingTime = timer->getCountDown();
	cout << game->getMaxTime() << endl;
	cout << remainingTime << endl;
	if(remainingTime < 10){
		numbers = numberToText(remainingTime);
		glPushMatrix();
			glTranslated(31.5, 4.5, 0);
			LSFrender::render(nodes,numbers,appearances,appearancesStack6,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}
	else{
		numbers = numberToText(remainingTime/10);
		glPushMatrix();
			glTranslated(30.5, 4.5, 0);
			LSFrender::render(nodes,numbers,appearances,appearancesStack6,animations,LSFscene::timeSeconds);
		glPopMatrix();
		numbers = numberToText(remainingTime%10);
		glPushMatrix();
			glTranslated(32.5, 4.5, 0);
			LSFrender::render(nodes,numbers,appearances,appearancesStack6,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}

	if(remainingTime <= 0){
		game->skipPlayer(s1);
		timer->stopCountDown();
	}

	// ---- END Primitive drawing section

	glutSwapBuffers();
}

string LSFscene::numberToText(int number){
	string str;
	switch(number){
	case 0: str = "zero"; break;
	case 1: str = "one"; break;
	case 2: str = "two"; break;
	case 3: str = "three"; break;
	case 4: str = "four"; break;
	case 5: str = "five"; break;
	case 6: str = "six"; break;
	case 7: str = "seven"; break;
	case 8: str = "eight"; break;
	case 9: str = "nine"; break;
	}

	return str;
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
