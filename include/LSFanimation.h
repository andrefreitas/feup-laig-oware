/****************************************************************************
 * Author:      - André Freitas, p.andrefreitas@gmail.com
 * Author:      - Paulo Araújo, paulojmaraujo@gmail.com
 * Copyright:   - 14/10/2012, LAIG, FEUP
 ****************************************************************************/

#ifndef LSFANIMATION_H_
#define LSFANIMATION_H_

#include <vector>
#include <math.h>
#include <iostream>

#include "LSFvertex.h"

class LSFanimation{
	protected:
		vector<LSFvertex> controlPoints; /* the animation control points */
		double totalTime; /* the total time of the animation */
		double totalPathLen; /* the length of the complete path */
		vector<LSFvertex> pathVelocity; /* the unit velocity vectors correspondent to each section v=x/t */
		vector<LSFvertex> pathDisplacement;
		vector<LSFvertex> pathRotation;
		vector<double> pathTime;
		vector<double> pathDistance;
	public:
		LSFanimation(vector<LSFvertex> controlPoints, double totalTime);
		LSFvertex getDisplacementAt(double elapsedTime);
		LSFvertex getRotationAt(double elapsedTime);
		int getPathAt(double &elapsedTime);

};


#endif
