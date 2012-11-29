/****************************************************************************
 * Author:      - André Freitas, p.andrefreitas@gmail.com
 * Author:      - Paulo Araújo, paulojmaraujo@gmail.com
 * Copyright:   - 14/10/2012, LAIG, FEUP
 ****************************************************************************/
#ifndef LSFPARSER_H
#define LSFPARSER_H

#define DEBUGMODE 0

#include "tinyxml.h"
#include <vector>
#include "CGFcamera.h"
#include "LSFnode.h"
#include "LSFobjects.h"
#include "CGFlight.h"
#include "LSFanimation.h"
#include <stack>
#include <iostream>
#include "LSFprimitive.h"
#include <map>
#include <CGFapplication.h>
#include "LSFvertex.h"
#include <iomanip>

using namespace std;
/**
 * globalsData is a struct that holds the globals configuration variables.
 */
struct globalsData{
	float background[4];
	const char *polygon_mode, *polygon_shading;
	const char *culling__frontfaceorder, *culling_cullface; bool culling_enabled;
};

/**
 * LSFparser: is the core class of the project. It parses the LSF scenes and put in know data structures,
 * like maps of LSF objects.
 */
class LSFparser{
	private:
		TiXmlDocument *lsfFile;
		TiXmlElement* lsfElement;
		TiXmlElement* globalsElement;
		TiXmlElement* camerasElement;
		TiXmlElement* graphElement;
		TiXmlElement* appearancesElement;
		TiXmlElement* lightingsElement;
		TiXmlElement* animationElement;
		int queryResult;
	public:
		LSFparser(char* a);
		void getGlobals(struct globalsData *globals);
		void getCameras(map<string, LSFcamera*> &cameras);
		void getNodes(map<string,LSFnode*> &nodes,string &rootNode);
		void getAppearances(map<string,LSFappearance*> &appearances);
		void getLights(map<string,LSFlight*> &lights, bool &enabled, bool &local, bool &doublesided,float *ambient);
		void buildDisplayLists(map<string,LSFnode*> &nodes,string &rootNode,map<string,LSFappearance*> appearances,stack<LSFappearance*> &appearancesStack, bool enabledDisplayList=0);
		void getAnimations(map<string,LSFanimation*> &animations);
};
#endif
