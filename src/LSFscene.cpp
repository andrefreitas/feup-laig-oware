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

	//create demoMode game
//	if(createGame(new Computer("ABC", "bot1"), new Human("123", "bot2"), 3))
//		loadingDemoMode = true;
//	else
//		exit(1);

//	loadingDemoMode = false;
//	createGame(new Computer("ABC", "human"), new Human("Paulo", "human"), 3);
//	startHumanVsHumanMode();

	loadingDemoMode = false;
	createGame(new Computer("ABC", "bot2"), new Human("Paulo", "human"), 3);
	startHumanVsComputerMode();
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

	if(loadingDemoMode){
		loadDemoMode();
	}
	else{
		drawBox();
		drawMarkers();

		if(demoTimer->isStarted())
			if(demoTimer->getCountDown() <= 0 && !gameStarted)
				startDemoMode();

		if(demoModeStarted)
			demoMode();
		else if(humanVsHumanModeStarted)
			humanVsHumanMode();
		else if(humanVsComputerModeStarted)
			humanVsComputerMode();

		//Players score and timer
		if(gameStarted){
			drawPlayerScore(game->getPlayer1()->getScore(), "1");
			drawPlayerScore(game->getPlayer2()->getScore(), "2");

			if(timer->isStarted() && game->getPlayer(atoi(game->getPlayerTurn().c_str()))->getType() == "human")
				drawRemainingTime(timer->getCountDown());
		}

		if(winnerFound && timer->getCountDown() < 5){
			timer->stopCountDown();
			winnerFound = false;
			demoModeStarted = false;
			gameStarted = false;
			humanVsHumanModeStarted = false;
			humanVsComputerModeStarted = false;
			demoTimer->startCountDown(15);
		}
	}

    glutSwapBuffers();
}

void LSFscene::selectionMode(){
	int displacement1[6]={56,42,29,16,2,-11};
	int displacement2[6]={-11,2,16,29,42,56};
	glPushName(-1);
	for (int unsigned i=0; i<6; i++){
		glLoadName(i);
		glPushMatrix();
			glTranslated(displacement1[i],0,27);
			selectionBox->draw();
		glPopMatrix();
	}

	for (int unsigned i=0; i<6; i++){
		glLoadName(i+6);
		glPushMatrix();
			glTranslated(displacement2[i],0,41);
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
	stack<LSFappearance*> appearancesStack;
	appearancesStack.push(defaultAppearance);
	LSFrender::render(nodes,markers,appearances,appearancesStack,animations,LSFscene::timeSeconds);
	glPopMatrix();
}

void LSFscene::drawBox(){
	string box = "Box";
	stack<LSFappearance*> appearancesStack;
	appearancesStack.push(defaultAppearance);
	LSFrender::render(nodes,box,appearances,appearancesStack,animations,LSFscene::timeSeconds);
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
		glScaled(1.5, 1.5, 1);
		if(playerTurn == "1")
			glTranslated(6, 14, 0.2);
		else
			glTranslated(6, 5, 0.2);
		LSFrender::render(nodes,numbers,appearances,appearancesStack4,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}
	else{
		numbers = numberToText(score/10);
		if(game->getPlayerTurn() == playerTurn)
			numbers.append("Y");
		glPushMatrix();
		glScaled(1.5, 1.5, 1);
		if(playerTurn == "1")
			glTranslated(5, 14, 0.1);
		else
			glTranslated(5, 5, 0.1);
		LSFrender::render(nodes,numbers,appearances,appearancesStack4,animations,LSFscene::timeSeconds);
		glPopMatrix();
		numbers = numberToText(score%10);
		if(game->getPlayerTurn() == playerTurn)
			numbers.append("Y");
		glPushMatrix();
		glScaled(1.5, 1.5, 1);
		if(playerTurn == "1")
			glTranslated(7, 14, 0.1);
		else
			glTranslated(7, 5, 0.1);
		LSFrender::render(nodes,numbers,appearances,appearancesStack4,animations,LSFscene::timeSeconds);
		glPopMatrix();
	}
}

void LSFscene::drawRemainingTime(int remainingTime){
	glPushMatrix();
	glScaled(1.5, 1.5, 1);
	drawNumber(remainingTime, 32, 8, 0.2, 1, 1, 1);
	glPopMatrix();

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
	appearances["seed"]->appearance->apply();
	game->drawSeeds(game->getBoard()->getPlayerSeeds(1), game->getPlayer1()->getScore(), 0, 0, 0);
	game->drawSeeds(game->getBoard()->getPlayerSeeds(2), game->getPlayer2()->getScore(), 94, 0, 14);

	int displacement[6]={0,13,27,40,53,66};
	for(int i = 1; i <= 6; i++)
		drawNumber(game->getBoard()->getPlayerSeeds(1).at(i-1),displacement[i-1]-9, 8, 23, 1, 1, 1);

	for(int i = 1; i <= 6; i++)
		drawNumber(game->getBoard()->getPlayerSeeds(2).at(i-1), displacement[i-1]-9, 8, 49, 1, 1, 1);
}

void LSFscene::drawClosedHand(bool active){
	int x = 0, y = 10, z = 0;
	int currentHole = game->getBoard()->getCurrentHole();
	string hand = "Hand10";
	stack<LSFappearance*> appearancesStackH;
	appearancesStackH.push(defaultAppearance);
	if(active){
		switch(currentHole){
		case  0: case 11: x = 64; break;
		case  1: case 10: x = 50; break;
		case  2: case  9: x = 36; break;
		case  3: case  8: x = 22; break;
		case  4: case  7: x =  7; break;
		case  5: case  6: x = -8; break;
		}

		if(currentHole > 5)
			z = 41;
		else
			z = 27;
	}
	else{
		x = 25; y = 8; z = 50;
	}

	glPushMatrix();
		glTranslated(x, y, z);
		appearances["seed"]->appearance->apply();
		game->drawHoleSeeds(game->getBoard()->getHoleseeds(), 0, 0, 0);
		LSFrender::render(nodes,hand,appearances,appearancesStackH,animations,LSFscene::timeSeconds);
	glPopMatrix();
}

string LSFscene::numberToText(int number){
	string str[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

	return str[number];
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

bool LSFscene::createGame(Player *player1, Player *player2, int dificultyLevel){
	game->createGame(player1, player2, dificultyLevel);
	return game->startGame(game->getPlayer1()->getType(), game->getPlayer2()->getType());
}

//demoMode methods
void LSFscene::loadDemoMode(){
//	cout << "Loading DemoMode" << endl;
//	game->loadDemoMode();
//	loadingDemoMode = false;
//	demoTimer->startCountDown(15);
	int num = game->readStatus();
	if(num == 0){
		loadingDemoMode = false;
		demoTimer->startCountDown(15);
	}
	else if(num == 1){
		game->swapPlayerTurn();
	}
	else if(num == 2){
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

	if(game->getBoard()->isLoaded())
		while(game->getBoard()->update());

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
	if(this->demoModeStarted){
		this->gameStarted = false;
		this->demoModeStarted = false;
		this->demoTimer->startCountDown(15);
	}
}

void LSFscene::demoMode(){
	if(!demoModeStatus.empty()){
		if(timer->isStarted()){
			if(timer->getCountDown() <= 9){
				if(!demoModeStatus.empty()){
					demoModeStatus.pop();
					demoModeChooses.pop();
					demoModePlayer1Seeds.pop();
					demoModePlayer2Seeds.pop();
				}

				timer->stopCountDown();
			}
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
		game->getPlayer(game->getDemoModeWinner() )->setScore(game->getDemoModeFinalPoints());

		winnerFound = true;
	}

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

	if(!winnerFound){
//				drawClosedHand(gameStarted);
		drawSeeds();
	}
}

//human vs computer mode
void LSFscene::startHumanVsComputerMode(){
	this->gameStarted = true;
	this->humanVsComputerModeStarted = true;
	this->demoModeStarted = false;
	this->winnerFound = false;
	this->demoTimer->stopCountDown();

	this->player1Score = 0;
	this->player2Score = 0;

	this->noSeeds = false;

	this->game->setPlayerTurn("1");

	game->readStatus();
	game->readStatus();
}

void LSFscene::humanVsComputerMode(){
	if(humanVsComputerModeStarted){
		if(game->getPlayer(atoi(game->getPlayerTurn().c_str()))->getType() != "human" &&
		   !game->getBoard()->isLoaded() && !winnerFound){
			cout << "reading bot" << endl;
			int num = game->readStatus();
			if(num == 0){
				cout << "bot num = 0" << endl;
				winnerFound = true;
				game->getPlayer(game->getWinner())->setScore(game->getFinalPoints());
				game->swapPlayerTurn();
				timer->startCountDown(game->getMaxTime());
			}
			else if(num == 1){
				cout << "bot num = 1" << endl;
				game->readStatus();
				game->swapPlayerTurn();
				game->update();

				game->setPlayerChoose(0);

				game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(), game->getPlayer2()->getSeeds(),
						atoi(game->getPlayerTurn().c_str()), 1);

				timer->startCountDown(game->getMaxTime());
			}
			else {
				cout << "bot num = 2 e 3" << endl;
				game->update();
				game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(), game->getPlayer2()->getSeeds(),
						atoi(game->getPlayerTurn().c_str()), game->getPlayerChoose());

				animationTimer->startCountDown(1);
			}
		}
		else if(game->getPlayer(atoi(game->getPlayerTurn().c_str()))->getType() == "human" &&
				!game->getBoard()->isLoaded() && !winnerFound){
			cout << "reading human" << endl;
			if(game->isNextStatusActive()){
				game->setGameStatus(game->getNextStatus());
				game->update();
				game->clearNextStatus();
				game->setPlayerChoose(0);

				game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(), game->getPlayer2()->getSeeds(),
						atoi(game->getPlayerTurn().c_str()), 1);

				timer->startCountDown(game->getMaxTime());
			}
			else{
				int num = game->readStatus();
				if(num == 0){
					cout << "human num = 0" << endl;
					winnerFound = true;
					game->update();
				}
				else if(num == 1){
					cout << "human num = 1" << endl;
					game->swapPlayerTurn();
					game->update();
				}
				else if(num == 2){
					cout << "human num = 2" << endl;
					game->update();
					cout << "atualizou" << endl;
					game->setPlayerChoose(0);
					game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(), game->getPlayer2()->getSeeds(),
							atoi(game->getPlayerTurn().c_str()), 1);

					timer->startCountDown(game->getMaxTime());
				}
			}
		}
	}

	if(!winnerFound){
		if(timer->isStarted()){
			if(timer->getCountDown() <= 0){
				if(game->isNextStatusActive()){
					game->setGameStatus(game->getNextStatus());
					game->update();
					game->clearNextStatus();
					game->setPlayerChoose(0);

					cout << "nextStatus active" << endl;
					game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(), game->getPlayer2()->getSeeds(),
							atoi(game->getPlayerTurn().c_str()), 1);

					timer->startCountDown(game->getMaxTime());
				}
				else{
					game->skipPlayer();
					cout << game->readStatus() << endl;
					cout << game->readStatus() << endl;
					cout << game->readStatus() << endl;
					game->update();

					game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(), game->getPlayer2()->getSeeds(),
							atoi(game->getPlayerTurn().c_str()), game->getPlayerChoose());

					animationTimer->startCountDown(1);
				}
			}
		}

		if(game->getPlayerChoose() != 0 &&
				game->getPlayer(atoi(game->getPlayerTurn().c_str()))->getType() == "human"){
			game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(), game->getPlayer2()->getSeeds(),
					atoi(game->getPlayerTurn().c_str()), game->getPlayerChoose());

			animationTimer->startCountDown(1);

			this->game->setPlayerChoose(0);
		}
		else if(animationTimer->isStarted()){
			if(game->getBoard()->isLoaded() && animationTimer->getCountDown() <= 0){
				if(game->getBoard()->update() == false){
					animationTimer->stopCountDown();
					game->swapPlayerTurn();
					if(!timer->isStarted() &&
							game->getPlayer(atoi(game->getPlayerTurn().c_str()))->getType() == "human"){
						timer->startCountDown(game->getMaxTime());
					}
				}
				else
					animationTimer->startCountDown(1);
			}
		}

		drawSeeds();
	}
	else
		game->swapPlayerTurn();
}

//human vs human mode
void LSFscene::startHumanVsHumanMode(){
	this->gameStarted = true;
	this->humanVsHumanModeStarted = true;
	this->demoModeStarted = false;
	this->winnerFound = false;
	this->demoTimer->stopCountDown();

	this->player1Score = 0;
	this->player2Score = 0;
	this->game->setPlayerChoose(0);
	this->noSeeds = false;

	game->readStatus();
	game->readStatus();
	game->update();

	if(!game->getBoard()->isLoaded() &&  !timer->isStarted()){
		game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(),
				                    game->getPlayer2()->getSeeds(), 1, 1);
	}

	timer->startCountDown(game->getMaxTime());
}

void LSFscene::humanVsHumanMode(){
	if(timer->isStarted()){
		if(timer->getCountDown() <= 0){
			cout << "timeout" << endl;
			game->skipPlayer();

			this->game->setPlayerChoose(0);

			if(noSeeds){
				game->update();
				noSeeds = false;
			}
			int num = game->readStatus();
			if(num == 1){
				cout << num << endl;
				noSeeds = true;
				game->readStatus();
				game->swapPlayerTurn();
				if(!game->getBoard()->isLoaded() &&  !timer->isStarted())
					game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(),
							game->getPlayer2()->getSeeds(), 1, 1);

				this->game->setPlayerChoose(0);
			}
			else{
				if(num != 2)
					game->readStatus();
				game->update();

				if(!game->getBoard()->isLoaded() &&  !timer->isStarted()){
					game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(),
							game->getPlayer2()->getSeeds(), 1, 1);
				}
			}
			timer->startCountDown(game->getMaxTime());
		}
	}

	if(game->getPlayerChoose() != 0){
		game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(), game->getPlayer2()->getSeeds(),
				atoi(game->getPlayerTurn().c_str()), game->getPlayerChoose());

		animationTimer->startCountDown(1);

		this->game->setPlayerChoose(0);
	}
	else if(animationTimer->isStarted()){
		if(game->getBoard()->isLoaded() && animationTimer->getCountDown() <= 0){
			if(game->getBoard()->update() == false){
				if(!timer->isStarted()){
					timer->startCountDown(game->getMaxTime());
					animationTimer->stopCountDown();
				}
				if(noSeeds){
					game->update();
					noSeeds = false;
				}
				int num = game->readStatus();
				if(num == 1){
					cout << num << endl;
					noSeeds = true;
					game->readStatus();
					game->swapPlayerTurn();
					if(!game->getBoard()->isLoaded() &&  !timer->isStarted())
						game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(),
								game->getPlayer2()->getSeeds(), 1, 1);

					this->game->setPlayerChoose(0);
				}
				else{
					if(num == 0){
						winnerFound = true;
						game->getPlayer(game->getWinner())->setScore(game->getFinalPoints());
					}
					else{
						cout << "reload" << endl;
						game->swapPlayerTurn();
						game->update();

						game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(),
								game->getPlayer2()->getSeeds(),
								atoi(game->getPlayerTurn().c_str()), 1);
					}
				}
				timer->startCountDown(game->getMaxTime());
			}
			else
				animationTimer->startCountDown(1);
		}
	}

	if(!winnerFound){
		drawSeeds();
	}
	else
		game->swapPlayerTurn();
}

void LSFscene::skipPlayer(){
	game->skipPlayer();

	this->game->setPlayerChoose(0);

	if(noSeeds){
		game->update();
		noSeeds = false;
	}
	int num = game->readStatus();
	if(num == 1){
		noSeeds = true;
		game->readStatus();
		game->swapPlayerTurn();
		if(!game->getBoard()->isLoaded() &&  !timer->isStarted())
			game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(),
					game->getPlayer2()->getSeeds(), 1, 1);

		this->game->setPlayerChoose(0);
	}
	else{
		if(num != 2)
			game->readStatus();
		game->update();

		if(!game->getBoard()->isLoaded() &&  !timer->isStarted()){
			game->getBoard()->loadBoard(game->getPlayer1()->getSeeds(),
					game->getPlayer2()->getSeeds(), 1, 1);
		}
	}

	timer->startCountDown(game->getMaxTime());
}

void LSFscene::boardHandler(int position){
	if(!demoModeStarted && !animationTimer->isStarted() && timer->isStarted()){
		if((game->getPlayerTurn() == "1" && position < 6) ||
		   (game->getPlayerTurn() == "2" && position > 5)){
			int hole[] = {6, 5, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6};

			cout << " playerTurn = " << game->getPlayerTurn() << endl;
			cout << " hole = " << hole[position] << endl;
			cout << " holeSeeds =  " << game->getPlayer2()->getHoleSeeds(hole[position]) << endl;

			// this function is called when a hole from the board is clicked
			if(game->getPlayer(atoi(game->getPlayerTurn().c_str()))->getHoleSeeds(hole[position]) > 0){
				timer->stopCountDown();
				game->play(position);
				game->setPlayerChoose(hole[position]);
			}
		}
	}
}
