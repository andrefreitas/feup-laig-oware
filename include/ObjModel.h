#ifndef OBJMODEL_H
#define OBJMODEL_H
/****************************************************************************
 * Author:      - André Freitas, p.andrefreitas@gmail.com
 * Author:      - Paulo Araújo, paulojmaraujo@gmail.com
 * Copyright:   - 14/10/2012, LAIG, FEUP
 *****************************************************************************/

#include "glm.h"
#include <string>
#include <string.h>
using namespace std;
/*
	ObjModel - A class for loading .obj models and redering it. It doesn't load materials
*/
class ObjModel{
protected:
	GLMmodel *model;
public:
	ObjModel(string filename);
	void draw();
};
#endif