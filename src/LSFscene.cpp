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

	//startGame
	game = new Oware(new Computer("ABC", "bot2"), new Human("Paulo", "human"), 3);

	cout << game->getRoules() << endl;

	while(game->startServer() != 0){
		cout << "\nWAITING FOR SERVER - 5 seconds sleeping\n" << endl;
		timer->wait(5);
	}
	game->startGame(game->getPlayer1()->getType(), game->getPlayer2()->getType());
	//game->startGame(game->getPlayer1()->getType(), game->getPlayer2()->getType(), "1", "[[1,2,3,4,5,6],[1,1,1,1,1,1]]", "10", "11");
	//game->startGame(s1, player1->getType(), player2->getType(), "1", "[[0,0,0,0,0,0],[0,0,0,0,0,0]]", "24", "24");

	//código para modificar
	game->readStatus();

	selectionBox=new LSFBox(0,7,0,7,0,7);
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
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	CGFscene::activeCamera->applyView();
	axis.draw();
	glPolygonMode(face, mode);

	 // Scenario		
	stack<LSFappearance*> appearancesStack;
	appearancesStack.push(defaultAppearance);
	LSFrender::render(nodes,scenario,appearances,appearancesStack,animations,LSFscene::timeSeconds); 

	// Board
	stack<LSFappearance*> appearancesStack2;
	appearancesStack2.push(defaultAppearance);
	string board="Board";
	LSFrender::render(nodes,board,appearances,appearancesStack2,animations,LSFscene::timeSeconds);


	//Player1 is bot1 or bot2
	if(game->getPlayer1()->getType() != "human" && game->getPlayerTurn() == "1"){
		game->readStatus();
		game->swapPlayerTurn();
	}

    // Markers
    glPushMatrix();
    string markers;
    if(game->getPlayerTurn() == "1")
    	markers="MarkersP1";
    else if(game->getPlayerTurn() == "2")
    	markers="MarkersP2";
    else
    	markers="Markers";
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
    	if(game->getPlayerTurn() == "1")
    		numbers.append("Y");
    	glPushMatrix();
    	glTranslated(6, 11, 0);
    	LSFrender::render(nodes,numbers,appearances,appearancesStack4,animations,LSFscene::timeSeconds);
    	glPopMatrix();
    }
    else{
    	int score = game->getPlayer1()->getScore();
    	numbers = numberToText(score/10);
    	if(game->getPlayerTurn() == "1")
    		numbers.append("Y");
    	glPushMatrix();
    	glTranslated(5, 11, 0);
    	LSFrender::render(nodes,numbers,appearances,appearancesStack4,animations,LSFscene::timeSeconds);
    	glPopMatrix();
    	numbers = numberToText(score%10);
    	if(game->getPlayerTurn() == "1")
    		numbers.append("Y");
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
    	if(game->getPlayerTurn() == "2")
    		numbers.append("Y");
    	glPushMatrix();
    	glTranslated(6, 2, 0);
    	LSFrender::render(nodes,numbers,appearances,appearancesStack5,animations,LSFscene::timeSeconds);
    	glPopMatrix();
    }
    else{
    	int score = game->getPlayer2()->getScore();
    	numbers = numberToText(score/10);
    	if(game->getPlayerTurn() == "2")
    		numbers.append("Y");
    	glPushMatrix();
    	glTranslated(5, 2, 0);
    	LSFrender::render(nodes,numbers,appearances,appearancesStack5,animations,LSFscene::timeSeconds);
    	glPopMatrix();
    	numbers = numberToText(score%10);
    	if(game->getPlayerTurn() == "2")
    		numbers.append("Y");
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
    	game->skipPlayer();
    	game->readStatus();
    	timer->stopCountDown();
    }

    // ---- END Primitive drawing section

    glutSwapBuffers();
}


void LSFscene::selectionMode(){
	glPushName(-1);
	for (int unsigned i=0; i<6; i++){
		glLoadName(i);
		glPushMatrix();
			glTranslated(43.5-i*8,0,22);
			selectionBox->draw();
		glPopMatrix();
	}

	for (int unsigned i=0; i<6; i++){
		glLoadName(i+6);
		glPushMatrix();
			glTranslated(3.5+i*8,0,30);
			selectionBox->draw();
		glPopMatrix();
	}
	glPopName();
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

void LSFscene::boardHandler(int position){
	// this function is called when a hole from the board is clicked
	cout << "clicked board\n";
}
