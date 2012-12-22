/****************************************************************************
 * Author:      - André Freitas, p.andrefreitas@gmail.com
 * Author:      - Paulo Araújo, paulojmaraujo@gmail.com
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
	LSFBox *selectionBox;
	void selectionMode();
	void boardHandler(int position);
};
#endif
