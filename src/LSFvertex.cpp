#include "LSFvertex.h"

using namespace std;
LSFvertex::LSFvertex(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

void LSFvertex::print(){
	cout <<"( "<< x << " , "<< y <<" , " << z << " )";
}
void LSFvertex::rotateY(double angleRad) {
	double xtemp = x; // one does not simply use the changed x in the z equation
	x = x * cos(angleRad) + z * sin(angleRad);
	z = -xtemp * sin(angleRad) + z * cos(angleRad);

}

void LSFvertex::rotateZ(double angleRad){
	double xtemp=x;
	x = x * cos(angleRad) - y * sin(angleRad);
	y = xtemp * sin(angleRad) + y * cos(angleRad);
}

void LSFvertex::translate(double dx, double dy, double dz){
	x=x+dx;
	y=y+dy;
	z=z+dz;
}
mySemiRect::mySemiRect(LSFvertex *begin, LSFvertex *end, double divideStep) {
	this->begin = begin;
	this->end = end;
	this->divideStep = divideStep;

	deltaX = end->x - begin->x;
	deltaX = deltaX / divideStep;

	deltaY = end->y - begin->y;
	deltaY = deltaY / divideStep;

	deltaZ = end->z - begin->z;
	deltaZ = deltaZ / divideStep;
}

LSFvertex mySemiRect::getVertexT(int t) {
	return LSFvertex(begin->x + deltaX * t, begin->y + deltaY * t,
			begin->z + deltaZ * t);
}

LSFvertex computeNormalNewel(vector<LSFvertex> vertexList) {
	// initial normal
	LSFvertex normal(0, 0, 0);
	for (int unsigned i = 0; i < vertexList.size(); i++) {
		LSFvertex current = vertexList[i];
		LSFvertex next = vertexList[(i + 1) / vertexList.size()];
		normal.x = normal.x + ((current.y - next.y) * (current.z + next.z));
		normal.y = normal.y + ((current.z - next.z) * (current.x + next.x));
		normal.z = normal.z + ((current.x - next.x) * (current.y + next.y));
	}
	return normal;

}

vector<LSFvertex> computeTriangleUV(vector<LSFvertex> vertexList){
	// Explanation: the algorithm forces the base to be 1. But, if the height exceeds 1, it reduces the others sides.
	LSFvertex vectorA; float normA;
	// Compute A
	vectorA.x=vertexList[1].x-vertexList[0].x;
	vectorA.y=vertexList[1].y-vertexList[0].y;
	vectorA.z=vertexList[1].z-vertexList[0].z;
	normA=sqrt(vectorA.x*vectorA.x + vectorA.y*vectorA.y + vectorA.z*vectorA.z);

	LSFvertex vectorB; //float normB;
	// Compute B
	vectorB.x=vertexList[2].x-vertexList[1].x;
	vectorB.y=vertexList[2].y-vertexList[1].y;
	vectorB.z=vertexList[2].z-vertexList[1].z;
	//normB=sqrt(vectorB.x*vectorB.x + vectorB.y*vectorB.y + vectorB.z*vectorB.z);

	LSFvertex vectorC; float normC;
	// Compute C
	vectorC.x=vertexList[2].x-vertexList[0].x;
	vectorC.y=vertexList[2].y-vertexList[0].y;
	vectorC.z=vertexList[2].z-vertexList[0].z;
	normC=sqrt(vectorC.x*vectorC.x + vectorC.y*vectorC.y + vectorC.z*vectorC.z);

	// Compute scale factor(base is 1)
	//float sideA;
	//float sideB;
	float sideC;
	float scale=1/(float)normA;
	//sideA=1.0;
	//sideB=scale*normB;
	sideC=scale*normC;

	// UV
	LSFvertex uv1(0,0,0);
	LSFvertex uv2(1,0,0);
	LSFvertex uv3(0,0,0); // To compute bellow

	// Compute the angle between vector A and C
	//cos(t) = (v.w) / (|v|.|w|)

	float cosT=(vectorA.x*vectorC.x + vectorA.y*vectorC.y + vectorA.z*vectorC.z)/(normA*normC);
	float angle=acos(cosT);

	// soo the uv3 coordinates is
	float height=sideC*sin(angle);
	float semiBase=sideC*cos(angle);

	uv3.x=semiBase;
	uv3.y=height;

	// Push Back the UV coords
	vector<LSFvertex> uvCoords;
	uvCoords.push_back(uv1);
	uvCoords.push_back(uv2);
	uvCoords.push_back(uv3);
	return uvCoords;
}

float computeNormBetween(LSFvertex p1,LSFvertex p2){
	LSFvertex vectorA;
	vectorA.x=p2.x-p1.x;
	vectorA.y=p2.y-p1.y;
	vectorA.z=p2.z-p1.z;
	return sqrt(vectorA.x*vectorA.x + vectorA.y*vectorA.y + vectorA.z*vectorA.z);
}

float computeTriangleHeight(LSFvertex p1,LSFvertex p2,LSFvertex p3){
	// Explanation: the algorithm forces the base to be 1. But, if the height exceeds 1, it reduces the others sides.
		vector<LSFvertex> vertexList;
		vertexList.push_back(p1);
		vertexList.push_back(p2);
		vertexList.push_back(p3);
		LSFvertex vectorA; float normA;
		// Compute A
		vectorA.x=vertexList[1].x-vertexList[0].x;
		vectorA.y=vertexList[1].y-vertexList[0].y;
		vectorA.z=vertexList[1].z-vertexList[0].z;
		normA=sqrt(vectorA.x*vectorA.x + vectorA.y*vectorA.y + vectorA.z*vectorA.z);

		LSFvertex vectorB; //float normB;
		// Compute B
		vectorB.x=vertexList[2].x-vertexList[1].x;
		vectorB.y=vertexList[2].y-vertexList[1].y;
		vectorB.z=vertexList[2].z-vertexList[1].z;
		//normB=sqrt(vectorB.x*vectorB.x + vectorB.y*vectorB.y + vectorB.z*vectorB.z);

		LSFvertex vectorC; float normC;
		// Compute C
		vectorC.x=vertexList[2].x-vertexList[0].x;
		vectorC.y=vertexList[2].y-vertexList[0].y;
		vectorC.z=vertexList[2].z-vertexList[0].z;
		normC=sqrt(vectorC.x*vectorC.x + vectorC.y*vectorC.y + vectorC.z*vectorC.z);

		// Compute scale factor(base is 1)
		//float sideA;
		//float sideB;
		float sideC;
		float scale=1/(float)normA;
		//sideA=1.0;
		//sideB=scale*normB;
		sideC=scale*normC;

		// UV
		LSFvertex uv1(0,0,0);
		LSFvertex uv2(1,0,0);
		LSFvertex uv3(0,0,0); // To compute bellow

		// Compute the angle between vector A and C
		//cos(t) = (v.w) / (|v|.|w|)

		float cosT=(vectorA.x*vectorC.x + vectorA.y*vectorC.y + vectorA.z*vectorC.z)/(normA*normC);
		float angle=acos(cosT);

		// soo the uv3 coordinates is
		float height=sideC*sin(angle);
		return height;
}

LSFvertex angleBetween(LSFvertex v1, LSFvertex v2){
	LSFvertex cross=crossProduct(v1,v2);
	LSFvertex rotation(0,0,0);
	float degreeAngle=computeAngleBetween(v1,v2);

	// They are on a plane
	if(cross.x==0 && cross.y==0)
		rotation.z=degreeAngle;
	else if(cross.x==0 && cross.z==0)
		rotation.y=degreeAngle;
	else if(cross.y==0 && cross.z==0)
		rotation.x=degreeAngle;
	else {

	// Else
	LSFvertex a;
	LSFvertex b;
	// Compute angle in x
	a=v1; a.x=0;
	b=v2; b.x=0;
	rotation.x=computeAngleBetween(a,b);

	// Compute angle in y
	a=v1; a.y=0;
	b=v2; b.y=0;
	rotation.y=computeAngleBetween(a,b);

	// Compute angle in z
	a=v1; a.z=0;
	b=v2; b.z=0;
	rotation.z=computeAngleBetween(a,b);
	}
	return rotation;
}

float computeAngleBetween(LSFvertex v1, LSFvertex v2){
	float normA=sqrt(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z);
	float normB=sqrt(v2.x*v2.x + v2.y*v2.y + v2.z*v2.z);
	if(normA==0 || normB==0) return 0;
	float cosT=(v1.x*v2.x + v1.y*v2.y + v1.z*v2.z)/(normA*normB);
	return acos(cosT)/acos(-1)*180;
}
LSFvertex crossProduct(LSFvertex v1, LSFvertex v2){
	LSFvertex cross(0,0,0);
	cross.x= v1.y*v2.z - v1.z*v2.y;
	cross.y-= (v1.x*v2.z - v1.z*v2.x);
	cross.z= v1.x*v2.y - v1.y*v2.x;
	return cross;

}
