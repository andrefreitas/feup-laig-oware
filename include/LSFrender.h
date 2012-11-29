/****************************************************************************
 * Author:      - André Freitas, p.andrefreitas@gmail.com
 * Author:      - Paulo Araújo, paulojmaraujo@gmail.com
 * Copyright:   - 14/10/2012, LAIG, FEUP
 ****************************************************************************/
#ifndef LSFRENDER_H_
#define LSFRENDER_H_

#include <map>
#include <string>
#include <stack>
#include <iostream>

#include "CGFapplication.h"

#include "LSFnode.h"
#include "LSFobjects.h"
#include "LSFanimation.h"
#include "LSFvertex.h"
#include "LSFprimitive.h"

/**
 * LSFrender is a class that receives all the scene objects and render it in OPENGL
 */
class LSFrender{
	public:
		static void render(map<string,LSFnode*> &nodes,string &rootNode,map<string,LSFappearance*> appearances,stack<LSFappearance*> &appearancesStack,map<string,LSFanimation*> animations, double timeSeconds);
};
#endif
