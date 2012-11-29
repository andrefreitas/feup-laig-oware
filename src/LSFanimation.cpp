#include "LSFanimation.h"

using namespace std;
#define DEBUG_ANIMATION 0
LSFanimation::LSFanimation(vector<LSFvertex> controlPoints, double totalTime) {
	this->controlPoints = controlPoints;
	this->totalTime = totalTime;
	this->totalPathLen = 0;

	// Compute the paths displacement and len
	for (int unsigned i = 0; i < (controlPoints.size() - 1); i++) {

		LSFvertex displacement;
		double x, y, z;
		x = displacement.x = (controlPoints[i + 1].x - controlPoints[i].x);
		y = displacement.y = (controlPoints[i + 1].y - controlPoints[i].y);
		z = displacement.z = (controlPoints[i + 1].z - controlPoints[i].z);
		pathDisplacement.push_back(displacement);

		double distance = sqrt(x * x + y * y + z * z);
		totalPathLen += distance;
		pathDistance.push_back(distance);

		if (DEBUG_ANIMATION)
			cout << i << " - r ( " << displacement.x << " , " << displacement.y
					<< " , " << displacement.z << " )\n";
	}
	if (DEBUG_ANIMATION)
		cout << "Total distance: " << this->totalPathLen << endl;

	// Compute the velocity vector in each path
	for (int unsigned i = 0; i < (controlPoints.size() - 1); i++) {

		pathTime.push_back(pathDistance[i] / totalPathLen * this->totalTime);

		LSFvertex velocity= pathDisplacement[i];
		velocity.x/=pathTime[i];
		velocity.y/=pathTime[i];
		velocity.z/=pathTime[i];
		pathVelocity.push_back(velocity);

		if (DEBUG_ANIMATION)
					cout << i << " - v ( " << velocity.x << " , " << velocity.y
							<< " , " << velocity.z << " )\n";
	}

	// Compute the rotation in each path
	pathRotation.push_back(LSFvertex(0,0,0));
	for (int unsigned i = 1; i < (controlPoints.size() - 1); i++) {
		LSFvertex rotation=angleBetween(pathDisplacement[i-1], pathDisplacement[i]);
		rotation.x+=pathRotation[i-1].x;
		rotation.y+=pathRotation[i-1].y;
		rotation.z+=pathRotation[i-1].z;

		// IF - or +
		LSFvertex cross=crossProduct(pathDisplacement[i-1],pathDisplacement[i]);
		if(cross.x<0)
			rotation.x=0-rotation.x;
		if(cross.y<0)
			rotation.y=0-rotation.y;
		if(cross.z<0)
			rotation.z=0-rotation.z;
		pathRotation.push_back(rotation);

	}
}
int LSFanimation::getPathAt(double &elapsedTime){
	double timeUntilNow=0;

	while(elapsedTime>totalTime) elapsedTime-=totalTime;

	for(int unsigned i=0; i<(controlPoints.size() -1);i++){
		//cout << pathTime[i] << endl;
		timeUntilNow+=pathTime[i];
		if(timeUntilNow>=elapsedTime) return i;

	}
	return -1;
}

LSFvertex LSFanimation::getDisplacementAt(double elapsedTime){
	// First from 0 to controlpoints -2, wich path i am?
	// (p1) ------ Path 0 ------ (p1) ------ Path 1 ------ (p1) ------ Path 2 ------ (p1)
	int path=getPathAt(elapsedTime);

	// Compute the sum of displacement vectors
	LSFvertex displacement(0,0,0);
	for(unsigned int i=0; i<path; i++){

		displacement.x+=pathDisplacement[i].x;
		displacement.y+=pathDisplacement[i].y;
		displacement.z+=pathDisplacement[i].z;

		elapsedTime-=pathTime[i];
	}

	displacement.x+=pathVelocity[path].x*elapsedTime;
	displacement.y+=pathVelocity[path].y*elapsedTime;
	displacement.z+=pathVelocity[path].z*elapsedTime;

	return displacement;

}
LSFvertex LSFanimation::getRotationAt(double elapsedTime){
	int path=getPathAt(elapsedTime);
	return pathRotation[path];
}
