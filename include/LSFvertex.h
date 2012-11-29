/****************************************************************************
 * Author:      - André Freitas, p.andrefreitas@gmail.com
 * Author:      - Paulo Araújo, paulojmaraujo@gmail.com
 * Copyright:   - 14/10/2012, LAIG, FEUP
 ****************************************************************************/

#ifndef LSFVERTEX_H_
#define LSFVERTEX_H_

#include <vector>
#include <math.h>
#include <iostream>

using namespace std;
/**
 * LSFvertex - a class for defining a vertex with his coordinates and basic transformations
 * x, y and z remains public for easy use
 */
class LSFvertex {
public:
	double x; /* x value */
	double y; /* y value */
	double z; /* z value */
	/**
	 * Constructor
	 * @param x the x value
	 * @param y the y value
	 * @param z the z value
	 */
	LSFvertex(){};
	LSFvertex(double x, double y, double z);
	/**
	 * Rotates the vertex in the XZ plane
	 * @param angleRad the angle in radians
	 */
	void rotateY(double angleRad);

	/**
	 * Rotates the vertex in the XY plane
	 * @param angleRad the angle in radians
	 */
	void rotateZ(double angleRad);

	/**
	 * Translates the point in 3 dimensions
	 * @param dx the x translation
	 * @param dy the y translation
	 * @param dz the z translation
	 */
	void translate(double dx, double dy, double dz);
	void print();
};

/**
 * mySemiRect - a class for defining a semirect that goes from a to b
 * We can get points between a and b that is very useful
 */
class mySemiRect {
	public:
	    LSFvertex *begin;		      /* pointer to begin vertex */
	    LSFvertex *end;				  /* pointer to end vertex */
		double divideStep; 			  /* it's a little difficult to explain, but think like this:  a+(b-a)/divideStep * t */
		double deltaX,deltaY,deltaZ; /* the step in every dimension */
		/**
		 * The constructor
		 * @param begin a pointer to the begin vertex
		 * @param end a pointer to the end vertex
		 * @param divideStep it's a little difficult to explain, but think like this:  a+(b-a)/divideStep * t
		 */
		mySemiRect(LSFvertex *begin, LSFvertex *end, double divideStep);
		/*
		 * Get a vertex in the semiRect
		 * @param t the increment over the vector
		 */
		LSFvertex getVertexT(int t);
};
/**
 * This function computes the normal of a polygon by giving the list of its vertex.
 * Implemented from: http://www.opengl.org/wiki/Calculating_a_Surface_Normal
 * @param vertexList the list of the polygon's vertexs
 * @return a LSFvertex object that is the normal
 */
LSFvertex computeNormalNewel(vector<LSFvertex> vertexList);

vector<LSFvertex> computeTriangleUV(vector<LSFvertex> vertexList);

float computeNormBetween(LSFvertex p1,LSFvertex p2);
float computeTriangleHeight(LSFvertex p1,LSFvertex p2,LSFvertex p3);
float computeAngleBetween(LSFvertex v1, LSFvertex v2);

LSFvertex angleBetween(LSFvertex v1, LSFvertex v2);
LSFvertex crossProduct(LSFvertex v1, LSFvertex v2);
#endif /* LSFVERTEX_H_ */
