#include "LSFscene.h"

#define DEMOMODETIME 5

using namespace std;
float pi = acos(-1.0);
float deg2rad=pi/180.0;

CGFappearance *mat1;

LSFscene::LSFscene(){}

double LSFscene::timeSeconds=0;

LSFscene::LSFscene(char* argv[]){
	if(argv[1] == NULL)
		inputScene=(char*)"../lsf/default.lsf";
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

	//startGame
	game = new Oware();

	while(game->startServer() != 0){
		cout << "\nWAITING FOR SERVER - 5 seconds sleeping\n" << endl;
		timer->wait(5);
	}

	gameStarted = false;

	timer = new Timer();
	demoTimer = new Timer();
	animationTimer = new Timer();

	selectionBox=new LSFBox(0,7,0,7,0,7);

/*
	if(createDemoMode())
		loadingDemoMode = true;
	else
		exit(1); 
		*/
		
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

	drawScenario();

	/*
	if(loadingDemoMode){
		loadDemoMode();
	}
	else{

		string box = "Box";
		stack<LSFappearance*> appearancesStack0;
		appearancesStack0.push(defaultAppearance);
		LSFrender::render(nodes,box,appearances,appearancesStack0,animations,LSFscene::timeSeconds);

		if(demoTimer->getCountDown() <= 0 && !gameStarted)
			startDemoMode();

		drawHand(gameStarted);

		if(demoModeStarted)
			demoMode();
	//	else

		//    else if(game->getPlayer1()->getType() != "human" && game->getPlayerTurn() == "1"){
		//		int num = game->readStatus();
		//		if(num == 2){
		//			timer->wait(3);
		//			game->readStatus();
		//		}
		//		game->update();
		//		game->swapPlayerTurn();
		//	}

		//drawMarkers();

		//Players seeds and timer
		if(gameStarted){
			if(!game->getBoard()->isLoaded() &&  !timer->isStarted()){
				game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(), game->getPlayer2()->getSeeds(),
						atoi(game->getPlayerTurn().c_str()), game->getPlayerChoose());

				animationTimer->startCountDown(1);
			}

			if(game->getBoard()->isLoaded() && animationTimer->getCountDown() <= 0){
				if(game->getBoard()->update() == false){
					if(!timer->isStarted()){
						timer->startCountDown(game->getMaxTime());
						animationTimer->stopCountDown();
					}
				}
				else
					animationTimer->startCountDown(1);
			}

			if(!winnerFound)
				drawSeeds();

			drawPlayerScore(game->getPlayer1()->getScore(), "1");

			drawPlayerScore(game->getPlayer2()->getScore(), "2");

			if(timer->isStarted() && !demoModeStarted)
				drawRemainingTime(timer->getCountDown());
		}

		if(demoModeEnd && timer->getCountDown() < 5){
			timer->stopCountDown();
			stopDemoMode();
			demoModeEnd = false;
		}
	}
 	
 	 */
	// Draw the current seeds
	appearances["seed"]->appearance->apply();
	game->drawSeeds();
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

void LSFscene::drawScenario(){
	 // Scenario
	stack<LSFappearance*> appearancesStack;
	appearancesStack.push(defaultAppearance);
	LSFrender::render(nodes,scenario,appearances,appearancesStack,animations,LSFscene::timeSeconds);

	// Board
	stack<LSFappearance*> appearancesStack2;
	appearancesStack2.push(defaultAppearance);
	string board="Board";
	LSFrender::render(nodes,board,appearances,appearancesStack2,animations,LSFscene::timeSeconds);
}

void LSFscene::drawMarkers(){
	glPushMatrix();
	string markers;
	if(gameStarted){
		if(game->getPlayerTurn() == "1")
			markers="MarkersP1";
		else if(game->getPlayerTurn() == "2")
			markers="MarkersP2";
	}
	else
	markers="Markers";
	stack<LSFappearance*> appearancesStack3;
	appearancesStack3.push(defaultAppearance);
	LSFrender::render(nodes,markers,appearances,appearancesStack3,animations,LSFscene::timeSeconds);
	glPopMatrix();
}

void LSFscene::drawPlayerScore(int score, string playerTurn){
	string numbers;
	stack<LSFappearance*> appearancesStack4;
	appearancesStack4.push(defaultAppearance);
	//Player seeds
	if(score < 10){
		numbers = numberToText(score);
		if(game->getPlayerTurn() == playerTurn)
			numbers.append("Y");
		glPushMatrix();
		if(playerTurn == "1")
			glTranslated(16, 21, 0.1);
		else
			glTranslated(16, 12, 0.1);
		LSFrender::render(nodes,numbers,appearances,appearancesStack4,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}
	else{
		numbers = numberToText(score/10);
		if(game->getPlayerTurn() == playerTurn)
			numbers.append("Y");
		glPushMatrix();
		if(playerTurn == "1")
			glTranslated(15, 21, 0.1);
		else
			glTranslated(15, 12, 0.1);
		LSFrender::render(nodes,numbers,appearances,appearancesStack4,animations,LSFscene::timeSeconds);
		glPopMatrix();
		numbers = numberToText(score%10);
		if(game->getPlayerTurn() == playerTurn)
			numbers.append("Y");
		glPushMatrix();
		if(playerTurn == "1")
			glTranslated(17, 21, 0.1);
		else
			glTranslated(17, 12, 0.1);
		LSFrender::render(nodes,numbers,appearances,appearancesStack4,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}
}

void LSFscene::drawRemainingTime(int remainingTime){
	drawNumber(remainingTime, 41.5, 14.5, 0.1, 1, 1, 1);

	if(remainingTime <= 0){
		game->skipPlayer();
		game->readStatus();
		game->update();
		timer->stopCountDown();
	}
}

void LSFscene::drawNumber(int number, int x, int y, int z, int sizeX, int sizeY, int sizeZ){
	string numbers;
	stack<LSFappearance*> appearancesStack;
	appearancesStack.push(defaultAppearance);
	if(number < 10){
		numbers = numberToText(number);
		glPushMatrix();
		glScaled(sizeX, sizeY, sizeZ);
		glTranslated(x, y, z);
		LSFrender::render(nodes,numbers,appearances,appearancesStack,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}
	else{
		numbers = numberToText(number/10);
		glPushMatrix();
		glScaled(sizeX, sizeY, sizeZ);
		glTranslated(x-sizeX, y, z);
		LSFrender::render(nodes,numbers,appearances,appearancesStack,animations,LSFscene::timeSeconds);
		glPopMatrix();
		numbers = numberToText(number%10);
		glPushMatrix();
		glScaled(sizeX, sizeY, sizeZ);
		glTranslated(x+sizeX, y, z);
		LSFrender::render(nodes,numbers,appearances,appearancesStack,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}
}

void LSFscene::drawSeeds(){
	for(int i = 1; i <= 6; i++)
		drawNumber(game->getBoard()->getPlayerSeeds(1).at(i-1), 7+(i-1)*8, 5, 21, 1, 1, 1);

	for(int i = 1; i <= 6; i++)
		drawNumber(game->getBoard()->getPlayerSeeds(2).at(i-1), 7+(i-1)*8, 5, 29, 1, 1, 1);
}

void LSFscene::drawHand(bool active){
	int x = 0, y = 0, z = 0;
	string hand = "Hand1";
	stack<LSFappearance*> appearancesStackH;
	appearancesStackH.push(defaultAppearance);
	if(active){
		switch(game->getBoard()->getCurrentHole()){
		case  0: x = 8.5+5*8; y = 7; z = 22; break;
		case  1: x = 8.5+4*8; y = 7; z = 22; break;
		case  2: x = 8.5+3*8; y = 7; z = 22; break;
		case  3: x = 8.5+2*8; y = 7; z = 22; break;
		case  4: x = 8.5+1*8; y = 7; z = 22; break;
		case  5: x = 8.5+0*8; y = 7; z = 22; break;
		case  6: x = 8.5+0*8; y = 7; z = 30; break;
		case  7: x = 8.5+1*8; y = 7; z = 30; break;
		case  8: x = 8.5+2*8; y = 7; z = 30; break;
		case  9: x = 8.5+3*8; y = 7; z = 30; break;
		case 10: x = 8.5+4*8; y = 7; z = 30; break;
		case 11: x = 8.5+5*8; y = 7; z = 30; break;
		}
	}
	else{
		x = 27; y = 3; z = 40;
	}
	glPushMatrix();
		glTranslated(x, y, z);
		LSFrender::render(nodes,hand,appearances,appearancesStackH,animations,LSFscene::timeSeconds);
	glPopMatrix();
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
	this->game->play(position);
}

bool LSFscene::createDemoMode(){
	game->createGame(new Computer("ABC", "bot1"), new Human("123", "bot2"), 3);
	return game->startGame(game->getPlayer1()->getType(), game->getPlayer2()->getType());
	//game->startGame(game->getPlayer1()->getType(), game->getPlayer2()->getType(), "1", "[[1,2,3,4,5,6],[1,1,1,1,1,1]]", "10", "11");
	//game->startGame(s1, player1->getType(), player2->getType(), "1", "[[0,0,0,0,0,0],[0,0,0,0,0,0]]", "24", "24");
//	return true;
}

void LSFscene::loadDemoMode(){
//	cout << "Loading DemoMode" << endl;
//	game->loadDemoMode();
//	loadingDemoMode = false;
//	demoTimer->startCountDown(15);
	int num = game->readStatus();
	if(num == 1){
		loadingDemoMode = false;
		demoTimer->startCountDown(15);
	}
	else if(num == 0){
		game->swapPlayerTurn();
	}
	else if(num == -1){
		game->update();
		game->swapPlayerTurn();
	}
	loading = "LoadGame";
	stack<LSFappearance*> appearancesStack0;
	appearancesStack0.push(defaultAppearance);
	LSFrender::render(nodes,loading,appearances,appearancesStack0,animations,LSFscene::timeSeconds);
}

void LSFscene::startDemoMode(){
	this->gameStarted = true;
	this->demoModeStarted = true;
	this->demoModeEnd = false;
	this->winnerFound = false;
	this->demoTimer->stopCountDown();

	this->player1Score = 0;
	this->player2Score = 0;

	this->demoModeStatus = game->getDemoModeStatus();
	this->demoModePlayer1Seeds = game->getdemoModePlayerSeeds(1);
	this->demoModePlayer2Seeds = game->getdemoModePlayerSeeds(2);
	this->demoModeChooses = game->getDemoModeChooses();

	if(!demoModeStatus.empty()){
		game->setPlayerTurn(demoModeStatus.front().at(0));
		game->getPlayer1()->setScore(atoi(demoModeStatus.front().at(2).c_str()));
		game->getPlayer2()->setScore(atoi(demoModeStatus.front().at(3).c_str()));
		game->setPlayerChoose(demoModeChooses.front());
	}

	if(!demoModePlayer1Seeds.empty()){
		game->getPlayer1()->setSeeds(demoModePlayer1Seeds.front());
		game->getPlayer2()->setSeeds(demoModePlayer2Seeds.front());
	}

	if(this->demoModeStatus.size() != this->demoModePlayer1Seeds.size() ||
	   this->demoModePlayer1Seeds.size() != this->demoModePlayer2Seeds.size() ||
	   this->demoModePlayer2Seeds.size() != this->demoModeChooses.size()){

		this->gameStarted = false;
		this->demoModeStarted = false;
		this->demoModeEnd = true;
		this->demoTimer->startCountDown(15);
	}

}

void LSFscene::stopDemoMode(){
	this->gameStarted = false;
	this->demoModeStarted = false;
	this->demoTimer->startCountDown(15);
}

void LSFscene::demoMode(){
	if(!demoModeStatus.empty()){
		if(timer->isStarted())
			if(timer->getCountDown() <= 9){
				if(!demoModeStatus.empty()){
					demoModeStatus.pop();
					demoModeChooses.pop();
					demoModePlayer1Seeds.pop();
					demoModePlayer2Seeds.pop();
				}

				timer->stopCountDown();
			}

		if(!demoModeStatus.empty()){
			game->setPlayerTurn(demoModeStatus.front().at(0));
			game->getPlayer1()->setScore(atoi(demoModeStatus.front().at(2).c_str()));
			game->getPlayer2()->setScore(atoi(demoModeStatus.front().at(3).c_str()));
			game->setPlayerChoose(demoModeChooses.front());
		}

		if(!demoModePlayer1Seeds.empty()){
			game->getPlayer1()->setSeeds(demoModePlayer1Seeds.front());
			game->getPlayer2()->setSeeds(demoModePlayer2Seeds.front());
		}

		if(animationTimer->isStarted() && animationTimer->getCountDown() <= 0)
			animationTimer->stopCountDown();
	}
	else if(demoModeStatus.empty()){
		game->swapPlayerTurn();
		if(game->getDemoModeWinner() == 1)
			game->getPlayer1()->setScore(game->getDemoModeFinalPoints());
		else
			game->getPlayer2()->setScore(game->getDemoModeFinalPoints());

		winnerFound = true;
		demoModeEnd = true;
	}
}
