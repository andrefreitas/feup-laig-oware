#include "LSFobjects.h"


void LSFcamera::updateProjectionMatrix(int width, int height){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float xScale = 1;
	float yScale = 1;

//	float ratio=(width/height);
//	if(ratio>1){
//		xScale=ratio;
//	}else{
//		yScale=ratio;
//	}

	if(view == "ortho"){
		glOrtho(left*xScale, right*xScale, bottom*yScale, top*yScale, _near, _far);
	}
	else{
		gluPerspective(angle, (width/(float)height), _near, _far);
	}

	glMatrixMode(GL_MODELVIEW);

}

void LSFcamera::setPosition(float x, float y, float z){
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

void LSFcamera::setTarget(float x, float y, float z){
	target[0] = x;
	target[1] = y;
	target[2] = z;
}

void LSFcamera::setStartRotation(){
	rotation[0] = 0;
	rotation[1] = 0;
	rotation[2] = 0;
}

void LSFcamera::translate(int axis, float value)
{
	if (axis==0 || axis==1 || axis==2)
		position[axis] += value;
}

void LSFcamera::applyView(){
	if(view == "perspective")
		gluLookAt(fromX, fromY, fromZ, toX, toY, toZ, 0, 1, 1);

}
