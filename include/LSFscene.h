/****************************************************************************
 * Author:      - Andr� Freitas, p.andrefreitas@gmail.com
 * Author:      - Paulo Ara�jo, paulojmaraujo@gmail.com
 * Copyright:   - 14/10/2012, LAIG, FEUP
 ****************************************************************************/

#ifndef LSFSCENE_H
#define LSFSCENE_H

#include <vector>
#include <map>
#include <math.h>
#include <iostream>

#include "CGFappearance.h"
#include "CGFapplication.h"
#include "CGFlight.h"
#include "CGFscene.h"
#include "CGFaxis.h"

#include "LSFnode.h"
#include "LSFparser.h"
#include "LSFrender.h"
#include "ObjModel.h"
#include "Oware.h"
#include "Timer.h"
#include "Board.h"

/**
 * LSFscene defines the scene parsed from the LSF file.
 */
class LSFscene : public CGFscene
{
	char* inputScene;
	LSFparser *sceneParser;
	struct globalsData globals;
	map<string,LSFappearance*> appearances;
	map<string,LSFnode*> nodes;
	map<string,LSFlight*> lights;
	map<string, LSFcamera*> cameras;
	map<string,LSFanimation*> animations;
	LSFappearance* defaultAppearance;
	string rootNode;

	bool lights_enabled, lights_local, lights_doublesided;
	float ambient[4];
	string activeCamera;
	GLenum face;
	GLenum mode;
	static double timeSeconds;

	Oware *game;
	Timer *timer;
	Timer *demoTimer;
	Timer *animationTimer;
	queue<vector<string> > demoModeStatus;
	queue<vector<int> > demoModePlayer1Seeds;
	queue<vector<int> > demoModePlayer2Seeds;
	queue<int> demoModeChooses;
	bool loadingDemoMode;
	bool gameStarted;
	bool demoModeStarted;
	bool humanVsHumanModeStarted;
	bool humanVsComputerModeStarted;
	bool demoModeEnd;
	bool winnerFound;
	bool noSeeds;
	int player1Score;
	int player2Score;

	int seedsStartPosition_y;
	int seedsCurrentPosition_y;

public:
	LSFscene();
	LSFscene(char* argv[]);
	~LSFscene();
	// --
	void init();
	void display();
	virtual void initCameras();
	void activateCamera(string id);
	// -----
	void setGlobals();
	void setPolygonMode(unsigned int, unsigned int);
	void positionView(string activeCamera, int axis, float newPosition);
	// --
	struct globalsData *getGlobals();
	map<string, LSFlight*> * getLights();
	map<string, LSFcamera*> * getCameras();
	void update(long millis);
	string numberToText(int number);
	string scenario;
	string loading;
	LSFBox *selectionBox;
	void selectionMode();
	void boardHandler(int position);
	void startDemoMode();
	void startHumanVsHumanMode();
	void startHumanVsComputerMode();
	void stopDemoMode();
	void demoMode();
	void humanVsHumanMode();
	void humanVsComputerMode();
	void skipPlayer();
	bool createDemoMode();
	bool createGame(Player *player1, Player *player2, int dificultyLevel);
	bool isDemoModeStarted();
	void loadDemoMode();
	void drawScenario();
	void drawMarkers();
	void drawBox();
	void drawPlayerScore(int score, string playerTurn);
	void drawRemainingTime(int remainingTime);
	void drawNumber(int number, int x, int y, int z, int sizeX, int sizeY, int sizeZ);
	void drawSeeds();
	void loadMovingSeeds();
	void drawMovingSeeds();
	void drawClosedHand();

};
#endif
