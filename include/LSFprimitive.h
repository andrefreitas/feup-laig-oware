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

class LSFprimitive{
public:
	virtual void init(LSFappearance *app) = 0;
	virtual void draw() = 0;
	virtual ~LSFprimitive();
};

class LSFRectangle: public LSFprimitive{
public:
	float x1, x2, y1, y2;
	float width, height;
	float u, v;

	LSFRectangle(float x1, float x2, float y1, float y2);
	void init(LSFappearance *app);
	void draw();
};

class LSFBox: public LSFprimitive{
public:
	float x1, x2, y1, y2, z1, z2;
	float width, height;
	float u, v;
	LSFRectangle *front, *back, *top, *bottom, *left, *right;

	LSFBox(float x1, float x2, float y1, float y2, float z1, float z2);
	void init(LSFappearance *app);
	void draw();
};

class LSFTriangle: public LSFprimitive{
public:
	float x1, x2, x3, y1, y2, y3, z1, z2, z3;
	float width, height;
	float u, v;
	LSFvertex p1,p2,p3;
	LSFvertex normal;
	vector<LSFvertex> uvCoords;

	LSFTriangle(float x1, float x2, float x3, float y1, float y2, float y3, float z1, float z2, float z3,
			    LSFvertex normal, vector<LSFvertex> uvCoords);
	void init(LSFappearance *app);
	void draw();
};

class LSFCylinder: public LSFprimitive{
public:
	float base, top, height;
	int slices, stacks;
	GLUquadric *a;

	LSFCylinder(float base, float top, float height, int slices, int stacks);
	void init(LSFappearance *app);
	void draw();
};

class LSFSphere: public LSFprimitive{
public:
	float radius;
	int slices, stacks;
	GLUquadric *a;

	LSFSphere(float radius, int slices, int stacks);
	void init(LSFappearance *app);
	void draw();
};

class LSFTorus: public LSFprimitive{
public:
	float inner, outer;
	int slices, loops;

	LSFTorus(float inner, float outer, int slices, int loops);
	void init(LSFappearance *app);
	void draw();
};

#endif /* LSFPRIMITIVE_H_ */
