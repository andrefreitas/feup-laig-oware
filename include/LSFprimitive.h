/****************************************************************************
 * Author:      - André Freitas, p.andrefreitas@gmail.com
 * Author:      - Paulo Araújo, paulojmaraujo@gmail.com
 * Copyright:   - 14/10/2012, LAIG, FEUP
 *****************************************************************************
 * This header defines all the possible primitives. We are
 * keeping the code "simple and sincere" and easy to change.
 ****************************************************************************/

#ifndef LSFPRIMITIVE_H_
#define LSFPRIMITIVE_H_

#include <iostream>
#include "LSFobjects.h"
#include "LSFvertex.h"
#include "LSFshader.h"

class Terrain{
	string heightmap;
	string texturemap;
	string fragmentshader;
	string vertexshader;
	LSFshader *shader;
	CGFshader *teste;
	public:
	Terrain(string heightmap, string texturemap, string fragmentshader, string vertexshader);
	void draw();

};
enum PrimitiveType{ rectangle,triangle,cylinder,sphere,torus,plane, patch, vehicle,terrain};
class LSFprimitive{
public:
	GLfloat *ctrlpoints;
	GLfloat *nrmlcompon;
	GLfloat *textpoints;
	GLfloat *colorpoints;

	LSFappearance *appearance;
	PrimitiveType type;
	map<string,float> attr;
	GLenum compute;
	LSFvertex normal;
	vector<LSFvertex> uvCoords;
	Terrain *terrain;
	LSFprimitive(PrimitiveType type);
	void createEvaluator(GLfloat *ctrlpoints, GLfloat *nrmlcompon, GLfloat *textpoints,
			             GLfloat *colorpoints, int startU, int startV);
	void drawBooster(int way, GLfloat *color);
	void drawBracket(float x, float y, float z, GLfloat *color);
	void drawWheel(float x, float y, float z, GLfloat *color);
	virtual void draw();
	virtual ~LSFprimitive();
};


#endif /* LSFPRIMITIVE_H_ */
