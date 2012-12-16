#include "LSFprimitive.h"

using namespace std;

LSFprimitive::~LSFprimitive() {
	// TODO Auto-generated destructor stub
}

LSFRectangle::LSFRectangle(float x1, float x2, float y1, float y2){
	this->x1 = x1;
	this->x2 = x2;
	this->y1 = y1;
	this->y2 = y2;
}

void LSFRectangle::init(LSFappearance *app){
	width = x2-x1;
	height = y2-y1;

	u = width/(float)app->length_s;
	v = height/(float)app->length_t;
}

void LSFRectangle::draw(){
	glNormal3f(0,0,1);
	glBegin(GL_QUADS);
		glTexCoord2d(0.0,0.0); glVertex3d(x1,y1,0);
		glTexCoord2d(u,0.0);   glVertex3d(x2,y1,0);
		glTexCoord2d(u,v);     glVertex3d(x2,y2,0);
		glTexCoord2d(0.0,v);   glVertex3d(x1,y2,0);
	glEnd();
}

LSFBox::LSFBox(float x1, float x2, float y1, float y2, float z1, float z2){
	this->x1 = x1;
	this->x2 = x2;
	this->y1 = y1;
	this->y2 = y2;
	this->z1 = z1;
	this->z2 = z2;

	front =  new LSFRectangle(-0.5, 0.5, -0.5, 0.5);
	back =   new LSFRectangle(-0.5, 0.5, -0.5, 0.5);
	top =    new LSFRectangle(-0.5, 0.5, -0.5, 0.5);
	bottom = new LSFRectangle(-0.5, 0.5, -0.5, 0.5);
	left =   new LSFRectangle(-0.5, 0.5, -0.5, 0.5);
	right =  new LSFRectangle(-0.5, 0.5, -0.5, 0.5);
}

void LSFBox::init(LSFappearance *app){
	front->init(app);
	back->init(app);
	top->init(app);
	bottom->init(app);
	left->init(app);
	right->init(app);
}

void LSFBox::draw(){
glPushMatrix();
	glScaled(abs(x2-x1), abs(y2-y1), abs(z2-z1));
	glPushMatrix();
		glTranslated(0, 0, 0.5);
		front->draw();
	glPopMatrix();
	glPushMatrix();
		glRotated(180, 0, 1, 0);
		glTranslated(0, 0, 0.5);
		back->draw();
	glPopMatrix();
	glPushMatrix();
		glRotated(-90, 1, 0, 0);
		glTranslated(0, 0, 0.5);
		top->draw();
	glPopMatrix();
	glPushMatrix();
		glRotated(90, 1, 0, 0);
		glTranslated(0, 0, 0.5);
		bottom->draw();
	glPopMatrix();
	glPushMatrix();
		glRotated(-90, 0, 1, 0);
		glTranslated(0, 0, 0.5);
		left->draw();
	glPopMatrix();
	glPushMatrix();
		glRotated(90, 0, 1, 0);
		glTranslated(0, 0, 0.5);
		right->draw();
	glPopMatrix();
glPopMatrix();
}

LSFTriangle::LSFTriangle(float x1, float x2, float x3, float y1, float y2, float y3, float z1, float z2, float z3,
		                 LSFvertex normal, vector<LSFvertex> uvCoords){
	this->x1 = x1;
	this->x2 = x2;
	this->x3 = x3;
	this->y1 = y1;
	this->y2 = y2;
	this->y3 = y3;
	this->z1 = z1;
	this->z2 = z2;
	this->z3 = z3;

	this->normal = normal;
	this->uvCoords = uvCoords;
}

void LSFTriangle::init(LSFappearance *app){

	p1 = LSFvertex(x1, y1, z1);
	p2 = LSFvertex(x2, y2, z2);
	p3 = LSFvertex(x3, y3, z3);

	width = computeNormBetween(p1,p2);
	height = computeTriangleHeight(p1,p2,p3);

	u = width/(float)app->length_s;
	v = height/(float)app->length_t;
}

void LSFTriangle::draw(){
	glNormal3f(normal.x, normal.y, normal.z);
	glBegin(GL_TRIANGLES);
		glTexCoord2d(0,0); // don't need s and t in the first coord
		glVertex3d(x1, y1, z1);
		glTexCoord2d(uvCoords[1].x*u, uvCoords[1].y*v);
		glVertex3d(x2, y2, z2);
		glTexCoord2d(uvCoords[2].x*u, uvCoords[2].y*v);
		glVertex3d(x3, y3, z3);
	glEnd();
}

LSFCylinder::LSFCylinder(float base, float top, float height, int slices, int stacks){
	this->base =   base;
	this->top =    top;
	this->height = height;
	this->slices = slices;
	this->stacks = stacks;
}

void LSFCylinder::init(LSFappearance *app){
	a = gluNewQuadric();
}

void LSFCylinder::draw(){
	gluQuadricNormals(a,GL_SMOOTH);
	gluQuadricTexture(a,GL_TRUE);
	gluCylinder(a, base, top, height, slices, stacks);
}

LSFSphere::LSFSphere(float radius, int slices, int stacks){
	this->radius = radius;
	this->slices = slices;
	this->stacks = stacks;
}

void LSFSphere::init(LSFappearance *app){
	a=gluNewQuadric();
}

void LSFSphere::draw(){
	gluQuadricNormals(a,GL_SMOOTH);
	gluQuadricTexture(a,GL_TRUE);
	gluSphere(a, radius, slices, stacks);
}

LSFTorus::LSFTorus(float inner, float outer, int slices, int loops){
	this->inner =  inner;
	this->outer =  outer;
	this->slices = slices;
	this->loops =  loops;
}

void LSFTorus::init(LSFappearance *app){

}

void LSFTorus::draw(){
	glutSolidTorus(inner, outer, slices, loops);
}
