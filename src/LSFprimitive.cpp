#include "LSFprimitive.h"

using namespace std;

LSFprimitive::LSFprimitive(PrimitiveType type):type(type){
	compute = GL_FILL;
	attr["order"] = 1;
	attr["partsU"] = 2;
	attr["partsV"] = 2;
}

void LSFprimitive::createEvaluator(GLfloat *ctrlpoints, GLfloat *nrmlcompon = NULL, GLfloat *textpoints = NULL,
                                   GLfloat *colorpoints = NULL, int startU = 0, int startV = 0){
	float u,v;
	u=1/(float)appearance->length_s;
	v=1/(float)appearance->length_t;
	int partsU, partsV;

	if(type == plane){
		partsU = partsV = attr["parts"];
	}
	else{
		partsU = attr["partsU"];
		partsV = attr["partsV"];
	}

	if(nrmlcompon == NULL) glFrontFace(GL_CW);

	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, (attr["order"]+1),
			                  0.0, 1.0, 3*(attr["order"]+1), (attr["order"]+1), ctrlpoints);

	if(textpoints == NULL){
		GLfloat texturespoints[4][2] = {{0.0, 0.0},{u, 0.0},{0.0, v},{u, v}};
		glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &texturespoints[0][0]);
	}
	else
		glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, textpoints);

	if(nrmlcompon != NULL)
		glMap2f(GL_MAP2_NORMAL, 0.0, 1.0, 3, (attr["order"]+1),
				                0.0, 1.0, 3*(attr["order"]+1), (attr["order"]+1), nrmlcompon);

	if(colorpoints != NULL)
		glMap2f(GL_MAP2_COLOR_4, 0.0, 1.0, 4, (attr["order"]+1),
						         0.0, 1.0, 4*(attr["order"]+1), (attr["order"]+1), colorpoints);
	else
		glColor3f(1.0,1.0,1.0);

	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_MAP2_TEXTURE_COORD_2);

	if(nrmlcompon == NULL)
		glEnable(GL_AUTO_NORMAL);
	else
		glEnable(GL_MAP2_NORMAL);

	if(colorpoints != NULL)
		glEnable(GL_MAP2_COLOR_4);

	glMapGrid2f(partsU, 0.0,1, partsV, 0.0,1);
	glEvalMesh2(compute, startU, partsU, startV, partsV);

	if(nrmlcompon == NULL) glFrontFace(GL_CCW);
}

void LSFprimitive::drawBooster(int way, GLfloat *color){
	if(way == 1 || way == -1){
	GLfloat BoosterR[16][3] = {
			{-1.5, -0.1, 1.7*way}, {-1.0, -0.1, 1.7*way}, {3.0, 0.0, 1.7*way},  {3.7, 0.0, 1.7*way},
			{-1.3, -0.4, 1.7*way}, {-1.0, -0.4, 1.7*way}, {3.0, -0.3, 1.7*way}, {3.4, -0.3, 1.7*way},
			{-1.1, -0.7, 1.7*way}, {-1.0, -0.7, 1.7*way}, {3.0, -0.6, 1.7*way}, {3.4, -0.6, 1.7*way},
			{-1.0, -0.8, 1.8*way}, {-1.0, -0.8, 1.8*way}, {3.0, -0.7, 1.8*way}, {3.7, -0.7, 1.8*way}};
	createEvaluator(&BoosterR[0][0], NULL, NULL, color);
	GLfloat BoosterRI[16][3] = {
			{-1.0, -0.8, 1.8*way}, {-1.0, -0.8, 1.8*way}, {3.0, -0.7, 1.8*way}, {3.7, -0.7, 1.8*way},
			{-1.1, -0.7, 1.7*way}, {-1.0, -0.7, 1.7*way}, {3.0, -0.6, 1.7*way}, {3.4, -0.6, 1.7*way},
			{-1.3, -0.4, 1.7*way}, {-1.0, -0.4, 1.7*way}, {3.0, -0.3, 1.7*way}, {3.4, -0.3, 1.7*way},
			{-1.5, -0.1, 1.7*way}, {-1.0, -0.1, 1.7*way}, {3.0, 0.0, 1.7*way},  {3.7, 0.0, 1.7*way}};
	createEvaluator(&BoosterRI[0][0], NULL, NULL, color);

	GLfloat BoosterB[16][3] = {
			{-1.0, -0.8, 1.8*way}, {-1.0, -0.8, 1.8*way}, {3.0, -0.7, 1.8*way}, {3.7, -0.7, 1.8*way},
			{-1.0, -0.9, 1.9*way}, {-1.0, -0.9, 1.9*way}, {3.0, -0.9, 2.0*way}, {3.4, -0.9, 1.9*way},
			{-1.0, -0.9, 3.0*way}, {-1.0, -0.9, 3.0*way}, {3.0, -0.9, 3.0*way}, {3.4, -0.9, 3.0*way},
			{-1.0, -1.0, 3.0*way}, {-1.0, -1.0, 3.0*way}, {3.0, -0.9, 3.0*way}, {3.7, -0.8, 3.1*way}};
	createEvaluator(&BoosterB[0][0], NULL, NULL, color);
	GLfloat BoosterBI[16][3] = {
			{-1.0, -1.0, 3.0*way}, {-1.0, -1.0, 3.0*way}, {3.0, -0.9, 3.0*way}, {3.7, -0.8, 3.1*way},
			{-1.0, -0.9, 3.0*way}, {-1.0, -0.9, 3.0*way}, {3.0, -0.9, 3.0*way}, {3.4, -0.9, 3.0*way},
			{-1.0, -0.9, 1.9*way}, {-1.0, -0.9, 1.9*way}, {3.0, -0.9, 2.0*way}, {3.4, -0.9, 1.9*way},
			{-1.0, -0.8, 1.8*way}, {-1.0, -0.8, 1.8*way}, {3.0, -0.7, 1.8*way}, {3.7, -0.7, 1.8*way}};
	createEvaluator(&BoosterBI[0][0], NULL, NULL, color);

	GLfloat BoosterL[16][3] = {
			{-1.0, -1.0, 3.0*way}, {-1.0, -1.0, 3.0*way}, {3.0, -0.9, 3.0*way}, {3.7, -0.8, 3.1*way},
			{-1.1, -0.9, 3.1*way}, {-1.0, -0.9, 3.1*way}, {3.0, -0.8, 3.1*way}, {3.4, -0.5, 3.1*way},
			{-1.3, -0.7, 3.1*way}, {-1.0, -0.7, 3.1*way}, {3.0, -0.5, 3.1*way}, {3.4, -0.4, 3.1*way},
			{-1.5, -0.4, 3.1*way}, {-1.0, -0.4, 3.1*way}, {3.0, -0.2, 3.1*way}, {3.7, -0.1, 3.1*way}};
	createEvaluator(&BoosterL[0][0], NULL, NULL, color);
	GLfloat BoosterLI[16][3] = {
			{-1.5, -0.4, 3.1*way}, {-1.0, -0.4, 3.1*way}, {3.0, -0.2, 3.1*way}, {3.7, -0.1, 3.1*way},
			{-1.3, -0.7, 3.1*way}, {-1.0, -0.7, 3.1*way}, {3.0, -0.5, 3.1*way}, {3.4, -0.4, 3.1*way},
			{-1.1, -0.9, 3.1*way}, {-1.0, -0.9, 3.1*way}, {3.0, -0.8, 3.1*way}, {3.4, -0.5, 3.1*way},
			{-1.0, -1.0, 3.0*way}, {-1.0, -1.0, 3.0*way}, {3.0, -0.9, 3.0*way}, {3.7, -0.8, 3.1*way}};
	createEvaluator(&BoosterLI[0][0], NULL, NULL, color);
	}

	GLfloat fire[16][4] = {{1,0,0,1},{1,0,0,1},{1,0,0,1},{1,0,0,1},
						   {1,0,0,1},{1,1,0,1},{1,1,0,1},{1,0,0,1},
						   {1,0,0,1},{1,1,0,1},{1,1,0,1},{1,0,0,1},
						   {1,0,0,1},{1,0,0,1},{1,0,0,1},{1,0,0,1}};
	GLfloat black[16][4] = {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},
			                 {0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},
			                 {0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},
			                 {0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1}};

	if(way == 1){
	GLfloat BoosterFront[16][3] = {
			{-1.0, -0.8, 1.8*way}, {-1.0, -0.9, 1.9*way}, {-1.0, -0.9, 2.8*way}, {-1.0, -1.0, 3.0*way},
			{-1.0, -0.7, 1.7*way}, {-1.0, -0.8, 1.9*way}, {-1.0, -0.8, 2.8*way}, {-1.0, -0.9, 3.1*way},
			{-1.0, -0.4, 1.7*way}, {-1.0, -0.3, 1.9*way}, {-1.0, -0.4, 2.8*way}, {-1.0, -0.7, 3.1*way},
			{-1.0, -0.1, 1.7*way}, {-1.0, -0.2, 1.9*way}, {-1.0, -0.3, 2.8*way}, {-1.0, -0.4, 3.1*way}};
	createEvaluator(&BoosterFront[0][0], NULL, NULL, &black[0][0]);

	GLfloat BoosterBack[16][3] = {
			{3.3, -0.8, 3.1*way}, {3.3, -0.9, 3.0*way}, {3.3, -0.9, 2.0*way}, {3.3, -0.7, 1.8*way},
			{3.0, -0.5, 3.1*way}, {3.0, -0.5, 3.0*way}, {3.0, -0.5, 2.0*way}, {3.0, -0.6, 1.7*way},
			{3.0, -0.4, 3.1*way}, {3.0, -0.4, 3.0*way}, {3.0, -0.4, 2.0*way}, {3.0, -0.4, 1.7*way},
			{3.4, -0.15, 3.1*way}, {3.4, -0.1, 3.0*way},{3.4, 0.0, 2.0*way},  {3.4, 0.0, 1.7*way}};
	createEvaluator(&BoosterBack[0][0], NULL, NULL, &fire[0][0]);
	}
	if(way == -1){
	GLfloat BoosterFront[16][3] = {
			{-1.0, -0.1, -1.7}, {-1.0, -0.2, -1.9}, {-1.0, -0.3, -2.8}, {-1.0, -0.4, -3.1},
			{-1.0, -0.4, -1.7}, {-1.0, -0.3, -1.9}, {-1.0, -0.4, -2.8}, {-1.0, -0.7, -3.1},
			{-1.0, -0.7, -1.7}, {-1.0, -0.8, -1.9}, {-1.0, -0.8, -2.8}, {-1.0, -0.9, -3.1},
			{-1.0, -0.8, -1.8}, {-1.0, -0.9, -1.9}, {-1.0, -0.9, -2.8}, {-1.0, -1.0, -3.0}};
	createEvaluator(&BoosterFront[0][0], NULL, NULL, &black[0][0]);

	GLfloat BoosterBack[16][3] = {
			{3.4, -0.15, -3.1}, {3.4, -0.1, -3.0}, {3.4, 0.0, -2.0},  {3.4, 0.0, -1.7},
			{3.0, -0.4, -3.1},  {3.0, -0.4, -3.0}, {3.0, -0.4, -2.0}, {3.0, -0.4, -1.7},
			{3.0, -0.5, -3.1},  {3.0, -0.5, -3.0}, {3.0, -0.5, -2.0}, {3.0, -0.6, -1.7},
			{3.3, -0.8, -3.1},  {3.3, -0.9, -3.0}, {3.3, -0.9, -2.0}, {3.3, -0.7, -1.8}};
	createEvaluator(&BoosterBack[0][0], NULL, NULL, &fire[0][0]);
	}
}

void LSFprimitive::drawBracket(float x, float y, float z, GLfloat *color){
	GLfloat WheelsBracketBack[16][3] = {
			{0.0+x, 0.0+y, 0.0+z},  {0.2+x, 0.0+y, 0.05+z},  {0.2+x, 0.0+y, 0.25+z},  {0.0+x, 0.0+y, 0.3+z},
			{0.0+x, -0.5+y, 0.0+z}, {0.2+x, -0.5+y, 0.05+z}, {0.2+x, -0.5+y, 0.25+z}, {0.0+x, -0.5+y, 0.3+z},
			{0.0+x, -1.1+y, 0.0+z}, {0.2+x, -1.1+y, 0.05+z}, {0.2+x, -1.1+y, 0.25+z}, {0.0+x, -1.1+y, 0.3+z},
			{0.0+x, -1.9+y, 0.0+z}, {0.2+x, -1.9+y, 0.05+z}, {0.2+x, -1.9+y, 0.25+z}, {0.0+x, -1.9+y, 0.3+z}};
	createEvaluator(&WheelsBracketBack[0][0], NULL, NULL, color);
	GLfloat WheelsBracketFront[16][3] = {
			{0.0+x, -1.9+y, 0.0+z}, {-0.2+x, -1.9+y, 0.05+z}, {-0.2+x, -1.9+y, 0.25+z}, {0.0+x, -1.9+y, 0.3+z},
			{0.0+x, -1.1+y, 0.0+z}, {-0.2+x, -1.1+y, 0.05+z}, {-0.2+x, -1.1+y, 0.25+z}, {0.0+x, -1.1+y, 0.3+z},
			{0.0+x, -0.5+y, 0.0+z}, {-0.2+x, -0.5+y, 0.05+z}, {-0.2+x, -0.5+y, 0.25+z}, {0.0+x, -0.5+y, 0.3+z},
			{0.0+x, 0.0+y, 0.0+z},  {-0.2+x, 0.0+y, 0.05+z},  {-0.2+x, 0.0+y, 0.25+z},  {0.0+x, 0.0+y, 0.3+z}};
	createEvaluator(&WheelsBracketFront[0][0], NULL, NULL, color);
	GLfloat WheelsBracketBottom[16][3] = {
			{0.0+x, -1.9+y, 0.0+z},   {0.0+x, -1.9+y, 0.0+z},   {0.0+x, -1.9+y, 0.0+z},  {0.0+x, -1.9+y, 0.0+z},
			{-0.2+x, -1.9+y, 0.05+z}, {-0.1+x, -1.9+y, 0.05+z}, {0.1+x, -1.9+y, 0.05+z}, {0.2+x, -1.9+y, 0.05+z},
			{-0.2+x, -1.9+y, 0.25+z}, {-0.1+x, -1.9+y, 0.25+z}, {0.1+x, -1.9+y, 0.25+z}, {0.2+x, -1.9+y, 0.25+z},
			{0.0+x, -1.9+y, 0.3+z},   {0.0+x, -1.9+y, 0.3+z},   {0.0+x, -1.9+y, 0.3+z},  {0.0+x, -1.9+y, 0.3+z}};
	createEvaluator(&WheelsBracketBottom[0][0], NULL, NULL, color);
}

void LSFprimitive::drawWheel(float x, float y, float z, GLfloat *color){
	GLfloat WheelFrontR[16][3] = {
			{0.0+x, 0.0+y, 0.0+z},    {0.0+x, 0.05+y, -0.05+z},   {0.0+x, 0.05+y, -0.15+z},   {0.0+x, 0.0+y, -0.2+z},
			{-0.3+x, -0.05+y, 0.0+z}, {-0.3+x, -0.05+y, -0.05+z}, {-0.3+x, -0.05+y, -0.15+z}, {-0.3+x, -0.05+y, -0.2+z},
			{-0.3+x, -0.35+y, 0.0+z}, {-0.3+x, -0.35+y, -0.05+z}, {-0.3+x, -0.35+y, -0.15+z}, {-0.3+x, -0.35+y, -0.2+z},
			{0.0+x, -0.4+y, 0.0+z},   {0.0+x, -0.45+y, -0.05+z},  {0.0+x, -0.45+y, -0.15+z},  {0.0+x, -0.4+y, -0.2+z}};
	createEvaluator(&WheelFrontR[0][0], NULL, NULL, color);
	GLfloat WheelBackR[16][3] = {
			{0.0+x, 0.0+y, -0.2+z},   {0.0+x, 0.05+y, -0.15+z},  {0.0+x, 0.05+y, -0.05+z},  {0.0+x, 0.0+y, 0.0+z},
			{0.3+x, -0.05+y, -0.2+z}, {0.3+x, -0.05+y, -0.15+z}, {0.3+x, -0.05+y, -0.05+z}, {0.3+x, -0.05+y, 0.0+z},
			{0.3+x, -0.35+y, -0.2+z}, {0.3+x, -0.35+y, -0.15+z}, {0.3+x, -0.35+y, -0.05+z}, {0.3+x, -0.35+y, 0.0+z},
			{0.0+x, -0.4+y, -0.2+z},  {0.0+x, -0.45+y, -0.15+z}, {0.0+x, -0.45+y, -0.05+z}, {0.0+x, -0.4+y, 0.0+z}};
	createEvaluator(&WheelBackR[0][0], NULL, NULL, color);

	GLfloat WheelR[16][3] = {
			{0.0+x, 0.0+y, -0.2+z},    {0.0+x, 0.0+y, -0.2+z},    {0.0+x, 0.0+y, -0.2+z},   {0.0+x, 0.0+y, -0.2+z},
			{-0.3+x, -0.05+y, -0.2+z}, {-0.1+x, -0.05+y, -0.2+z}, {0.1+x, -0.05+y, -0.2+z}, {0.3+x, -0.05+y, -0.2+z},
			{-0.3+x, -0.35+y, -0.2+z}, {-0.1+x, -0.35+y, -0.2+z}, {0.1+x, -0.35+y, -0.2+z}, {0.3+x, -0.35+y, -0.2+z},
			{0.0+x, -0.4+y, -0.2+z},   {0.0+x, -0.4+y, -0.2+z},   {0.0+x, -0.4+y, -0.2+z},  {0.0+x, -0.4+y, -0.2+z}};
	createEvaluator(&WheelR[0][0], NULL, NULL, color);
	GLfloat WheelL[16][3] = {
			{0.0+x, -0.4+y, 0.0+z},   {0.0+x, -0.4+y, 0.0+z},   {0.0+x, -0.4+y, 0.0+z},  {0.0+x, -0.4+y, 0.0+z},
			{-0.3+x, -0.35+y, 0.0+z}, {-0.1+x, -0.35+y, 0.0+z}, {0.1+x, -0.35+y, 0.0+z}, {0.3+x, -0.35+y, 0.0+z},
			{-0.3+x, -0.05+y, 0.0+z}, {-0.1+x, -0.05+y, 0.0+z}, {0.1+x, -0.05+y, 0.0+z}, {0.3+x, -0.05+y, 0.0+z},
			{0.0+x, 0.0+y, 0.0+z},    {0.0+x, 0.0+y, 0.0+z},    {0.0+x, 0.0+y, 0.0+z},   {0.0+x, 0.0+y, 0.0+z}};
	createEvaluator(&WheelL[0][0], NULL, NULL, color);
}

void LSFprimitive::draw(){
	switch(type){
		case rectangle:{
			glNormal3f(0,0,1);
			glBegin(GL_QUADS);
				float width= attr["x2"]-attr["x1"];
				float height= attr["y2"]-attr["y1"];
				float u,v;
				u=width/(float)appearance->length_s;
				v=height/(float)appearance->length_t;
				glTexCoord2d(0.0,0.0); glVertex3d(attr["x1"],attr["y1"],0);
				glTexCoord2d(u,0.0); glVertex3d(attr["x2"],attr["y1"],0);
				glTexCoord2d(u,v); glVertex3d(attr["x2"],attr["y2"],0);
				glTexCoord2d(0.0,v); glVertex3d(attr["x1"],attr["y2"],0);

			glEnd();
		}break;
		case triangle:{
			glNormal3f(normal.x, normal.y, normal.z);
			glBegin(GL_TRIANGLES);

				// Compute the height and width of the triangle
				LSFvertex p1,p2,p3;
				p1=LSFvertex(attr["x1"], attr["y1"], attr["z1"]);
				p2=LSFvertex(attr["x2"], attr["y2"], attr["z2"]);
				p3=LSFvertex(attr["x3"], attr["y3"], attr["z3"]);
				float width=computeNormBetween(p1,p2);
				float sFactor=(width/appearance->length_s);
				float height=computeTriangleHeight(p1,p2,p3);
				float tFactor=(height/appearance->length_t);
				// -->
				glTexCoord2d(0,0); // don't need s and t in the first coord
				glVertex3d(attr["x1"], attr["y1"], attr["z1"]);

				glTexCoord2d(uvCoords[1].x*sFactor, uvCoords[1].y*tFactor);
				glVertex3d(attr["x2"], attr["y2"], attr["z2"]);

				glTexCoord2d(uvCoords[2].x*sFactor, uvCoords[2].y*tFactor);
				glVertex3d(attr["x3"], attr["y3"], attr["z3"]);
			glEnd();
		}break;
		case cylinder:{
			GLUquadric *a=gluNewQuadric();
			gluQuadricNormals(a,GL_SMOOTH);
			gluQuadricTexture(a,GL_TRUE);
			gluCylinder(a, attr["base"], attr["top"], attr["height"], attr["slices"], attr["stacks"]);
		} break;
		case sphere:{
			GLUquadric *a=gluNewQuadric();
			gluQuadricNormals(a,GL_SMOOTH);
			gluQuadricTexture(a,GL_TRUE);
			gluSphere(a, attr["radius"], attr["slices"], attr["stacks"]);
		}break;
		case torus:{

			glutSolidTorus(attr["inner"], attr["outer"], attr["slices"], attr["loops"]);
		} break;
		case patch:{
			createEvaluator(ctrlpoints);
		}break;
		case plane:{
			GLfloat controlpoints[4][3] = {
								{ -0.5, 0, 0.5},{ 0.5, 0, 0.5},
								{-0.5, 0, -0.5},{ 0.5, 0, -0.5}};

			createEvaluator(&controlpoints[0][0]);
		}break;
		case vehicle:{
			attr["order"] = 3;
			attr["partsU"] = 20;
			attr["partsV"] = 20;
			textpoints = NULL;

			GLfloat RGB[16][4] = {{1,0.5,0.5,1},{0.5,0.5,0.5,1},{0.5,0.5,1,1},{0.5,1,0.5,1},
								 {0.5,1,0.5,1},{1,0.5,0.5,1},{0.5,0.5,0.5,1},{0.5,0.5,1,1},
								 {0.5,0.5,1,1},{0.5,1,0.5,1},{1,0.5,0.5,1},{0.5,0.5,0.5,1},
								 {0.5,0.5,0.5,1},{0.5,0.5,1,1},{0.5,1,0.5,1},{1,0.5,0.5,1}};

			GLfloat black[16][4] = {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},
					                 {0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},
					                 {0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},
					                 {0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1}};

			GLfloat grey[16][4] = {{0.3,0.3,0.3,1},{0.3,0.3,0.3,1},{0.3,0.3,0.3,1},{0.3,0.3,0.3,1},
									{0.3,0.3,0.3,1},{0.3,0.3,0.3,1},{0.3,0.3,0.3,1},{0.3,0.3,0.3,1},
									{0.3,0.3,0.3,1},{0.3,0.3,0.3,1},{0.3,0.3,0.3,1},{0.3,0.3,0.3,1},
									{0.3,0.3,0.3,1},{0.3,0.3,0.3,1},{0.3,0.3,0.3,1},{0.3,0.3,0.3,1}};

			GLfloat red[16][4] = {{1,0,0,1},{1,0,0,1},{1,0,0,1},{1,0,0,1},
					                 {1,0,0,1},{1,0,0,1},{1,0,0,1},{1,0,0,1},
					                 {1,0,0,1},{1,0,0,1},{1,0,0,1},{1,0,0,1},
					                 {1,0,0,1},{1,0,0,1},{1,0,0,1},{1,0,0,1}};

			GLfloat body_color[16][4] = {{1,1,1,1},{0,1,0,1},{1,1,0,1},{1,0,0,1},
					                     {1,1,1,1},{0,1,0,1},{0,1,0,1},{1,1,0,1},
					                     {1,1,1,1},{0,1,0,1},{0,1,0,1},{1,1,0,1},
					                     {1,1,1,1},{0,1,0,1},{1,1,0,1},{1,0,0,1}};

			GLfloat VW_color[16][4] = {{1,1,0,1},{1,1,0,1},{1,0,0,1},{1,0,0,1},
					                   {1,1,0,1},{1,1,0,1},{1,0,0,1},{1,0,0,1},
					                   {1,1,0,1},{1,1,0,1},{1,0,0,1},{1,0,0,1},
					                   {1,1,0,1},{1,1,0,1},{1,0,0,1},{1,0,0,1}};

			GLfloat green[16][4] = {{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1},
					                 {0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1},
					                 {0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1},
					                 {0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1}};

			GLfloat cockpit[16][4] = {{0.8,0.8,0.8,1},{0.8,0.8,0.8,1},{1,1,1,1},{0.6,0.6,0.6,1},
					                  {0.8,0.8,0.8,1},{0.8,0.8,0.8,1},{0,0,0,1},{0,0,0,1},
					                  {0.8,0.8,0.8,1},{0.8,0.8,0.8,1},{0,0,0,1},{0,0,0,1},
					                  {0.8,0.8,0.8,1},{0.8,0.8,0.8,1},{1,1,1,1},{0.6,0.6,0.6,1}};

			GLfloat bottom_color[16][4] = {{0.5,0.5,0.5,1},{0.5,0.5,0.5,1},{0.5,0.5,0.5,1},{0.5,0.5,0.5,1},
					                       {0.5,0.5,0.5,1},{0.5,0.5,0.5,1},{0.5,0.5,0.5,1},{0.5,0.5,0.5,1},
					                       {0.5,0.5,0.5,1},{0.5,0.5,0.5,1},{0.5,0.5,0.5,1},{0.5,0.5,0.5,1},
					                       {0.5,0.5,0.5,1},{0.5,0.5,0.5,1},{0.5,0.5,0.5,1},{0.5,0.5,0.5,1}};

			//body
			GLfloat topFrontBodypoints[16][3] = {
					{-16.5, -0.5, 0.0}, {-15.8, -0.3, 0.2}, {-15.1, 0.0, 0.3},  {-14.0, 0.0, 0.6},
					{-16.5, -0.5, 0.0}, {-15.8, 0.0, 0.1},  {-15.1, 0.6, 0.2},  {-14.0, 1.1, 0.4},
					{-16.5, -0.5, 0.0}, {-15.8, 0.0, -0.1}, {-15.1, 0.6, -0.2}, {-14.0, 1.1, -0.4},
					{-16.5, -0.5, 0.0}, {-15.8, -0.3, -0.2},{-15.1, 0.0, -0.3}, {-14.0, 0.0, -0.6}};
			createEvaluator(&topFrontBodypoints[0][0], NULL, NULL, &cockpit[0][0]);

			GLfloat topCenterBodypoints[16][3] = {
					{-14.0, 0.0, 0.6},  {-12.0, 0.0, 0.7},  {-5.0, 0.0, 0.7},  {5.0, 0.0, 0.7},
					{-14.0, 1.1, 0.4},  {-12.0, 1.4, 0.4},  {-5.0, 1.4, 0.4},  {5.0, 1.4, 0.4},
					{-14.0, 1.1, -0.4}, {-12.0, 1.4, -0.4}, {-5.0, 1.4, -0.4}, {5.0, 1.4, -0.4},
					{-14.0, 0.0, -0.6}, {-12.0, 0.0, -0.7}, {-5.0, 0.0, -0.7}, {5.0, 0.0, -0.7}};
			createEvaluator(&topCenterBodypoints[0][0], NULL, NULL, &body_color[0][0]);

			GLfloat topBackBodypoints[16][3] = {
					{5.0, 0.0, 0.7},  {7.0, 0.3, 0.5},  {9.0, 0.6, 0.3},  {10.0, 1.0, 0.0},
					{5.0, 1.4, 0.4},  {7.0, 1.3, 0.2},  {9.0, 1.3, 0.1},  {10.0, 1.0, 0.0},
					{5.0, 1.4, -0.4}, {7.0, 1.3, -0.2}, {9.0, 1.3, -0.1}, {10.0, 1.0, 0.0},
					{5.0, 0.0, -0.7}, {7.0, 0.3, -0.5}, {9.0, 0.6, -0.3}, {10.0, 1.0, 0.0}};
			createEvaluator(&topBackBodypoints[0][0], NULL, NULL, &black[0][0]);

			GLfloat bottomFrontBodypoints[16][3] = {
					{-16.5, -0.5, 0.0}, {-15.8, 0.0, -0.2},  {-15.1, 0.0, -0.3},  {-14.0, 0.0, -0.6},
					{-16.5, -0.5, 0.0}, {-15.8, -0.6, -0.1}, {-15.1, -0.4, -0.2}, {-14.0, -0.5, -0.4},
					{-16.5, -0.5, 0.0}, {-15.8, -0.6, 0.1},  {-15.1, -0.4, 0.2},  {-14.0, -0.5, 0.4},
					{-16.5, -0.5, 0.0}, {-15.8, 0.0, 0.2},   {-15.1, 0.0, 0.3},   {-14.0, 0.0, 0.6}};
			createEvaluator(&bottomFrontBodypoints[0][0], NULL, NULL, &bottom_color[0][0]);

			GLfloat bottomCenterBodypoints[16][3] = {
					{-14.0, 0.0, -0.6},  {-12.0, 0.0, -0.7},  {-5.0, 0.0, -0.7},  {5.0, 0.0, -0.7},
					{-14.0, -0.5, -0.4}, {-12.0, -0.5, -0.4}, {-5.0, -0.5, -0.4}, {5.0, -0.5, -0.4},
					{-14.0, -0.5, 0.4},  {-12.0, -0.5, 0.4},  {-5.0, -0.5, 0.4},  {5.0, -0.5, 0.4},
					{-14.0, 0.0, 0.6},   {-12.0, 0.0, 0.7},   {-5.0, 0.0, 0.7},   {5.0, 0.0, 0.7}};
			createEvaluator(&bottomCenterBodypoints[0][0], NULL, NULL, &bottom_color[0][0]);

			GLfloat bottomBackBodypoints[16][3] = {
					{5.0, 0.0, -0.7},  {7.0, 0.3, -0.5}, {9.0, 0.6, -0.3}, {10.0, 1.0, 0.0},
					{5.0, -0.5, -0.4}, {7.0, 0.0, -0.2}, {9.0, 0.5, -0.1}, {10.0, 0.8, 0.0},
					{5.0, -0.5, 0.4},  {7.0, 0.0, 0.2},  {9.0, 0.5, 0.1},  {10.0, 0.8, 0.0},
					{5.0, 0.0, 0.7},   {7.0, 0.3, 0.5},  {9.0, 0.6, 0.3},  {10.0, 1.0, 0.0}};
			createEvaluator(&bottomBackBodypoints[0][0], NULL, NULL, &bottom_color[0][0]);

			//wings
			GLfloat topFrontWings[16][3] = {
					{-8.5, 0.0, 0.7},  {-7.0, -0.1, 1.1},  {-5.5, -0.1, 1.6},  {-4.0, -0.2, 2.1},
					{-8.5, 0.1, 0.4},  {-7.0, 0.2, 0.4},   {-5.5, 0.2, 0.4},   {-4.0, 0.3, 0.4},
					{-8.5, 0.1, -0.4}, {-7.0, 0.2, -0.4},  {-5.5, 0.2, -0.4},  {-4.0, 0.3, -0.4},
					{-8.5, 0.0, -0.7}, {-7.0, -0.1, -1.1}, {-5.5, -0.1, -1.6}, {-4.0, -0.2, -2.1}};
			createEvaluator(&topFrontWings[0][0], NULL, NULL, &green[0][0]);

			GLfloat topCenterWings[16][3] = {
					{-4.0, -0.2, 2.1},  {-2.5, -0.3, 2.6},  {-1.0, -0.5, 3.3},  {0.5, -0.5, 4.4},
					{-4.0, 0.3, 0.4},   {-2.5, 0.3, 0.4},   {-1.0, 0.3, 0.4},   {0.5, 0.3, 0.4},
					{-4.0, 0.3, -0.4},  {-2.5, 0.3, -0.4},  {-1.0, 0.3, -0.4},  {0.5, 0.3, -0.4},
					{-4.0, -0.2, -2.1}, {-2.5, -0.3, -2.6}, {-1.0, -0.5, -3.3}, {0.5, -0.5, -4.4}};
			createEvaluator(&topCenterWings[0][0], NULL, NULL, &red[0][0]);

			GLfloat topBackWings[16][3] = {
					{0.5, -0.5, 4.4},  {2.0, -0.5, 5.6},  {3.0, -0.5, 6.0},  {3.5, -0.5, 5.5},
					{0.5, 0.3, 0.4},   {2.0, 0.3, 0.4},   {3.0, 0.3, 0.4},   {3.5, 0.3, 0.4},
					{0.5, 0.3, -0.4},  {2.0, 0.3, -0.4},  {3.0, 0.3, -0.4},  {3.5, 0.3, -0.4},
					{0.5, -0.5, -4.4}, {2.0, -0.5, -5.6}, {3.0, -0.5, -6.0}, {3.5, -0.5, -5.5}};
			createEvaluator(&topBackWings[0][0], NULL, NULL, &red[0][0]);

			GLfloat bottomFrontWings[16][3] = {
					{-8.5, 0.0, -0.7}, {-7.0, -0.1, -1.1}, {-5.5, -0.1, -1.6}, {-4.0, -0.2, -2.1},
					{-8.5, 0.1, -0.4}, {-7.0, 0.0, -0.4},  {-5.5, 0.0, -0.4},  {-4.0, 0.0, -0.4},
					{-8.5, 0.1, 0.4},  {-7.0, 0.0, 0.4},   {-5.5, 0.0, 0.4},   {-4.0, 0.0, 0.4},
					{-8.5, 0.0, 0.7},  {-7.0, -0.1, 1.1},  {-5.5, -0.1, 1.6},  {-4.0, -0.2, 2.1}};
			createEvaluator(&bottomFrontWings[0][0], NULL, NULL, &bottom_color[0][0]);

			GLfloat bottomCenterWings[16][3] = {
					{-4.0, -0.2, -2.1}, {-2.5, -0.3, -2.6}, {-1.0, -0.5, -3.3}, {0.5, -0.5, -4.4},
					{-4.0, 0.0, -0.4},  {-2.5, 0.0, -0.4},  {-1.0, -0.1, -0.4}, {0.5, -0.1, -0.4},
					{-4.0, 0.0, 0.4},   {-2.5, 0.0, 0.4},   {-1.0, -0.1, 0.4},  {0.5, -0.1, 0.4},
					{-4.0, -0.2, 2.1},  {-2.5, -0.3, 2.6},  {-1.0, -0.5, 3.3},  {0.5, -0.5, 4.4}};
			createEvaluator(&bottomCenterWings[0][0], NULL, NULL, &bottom_color[0][0]);

			GLfloat bottomBackWings[16][3] = {
					{0.5, -0.5, -4.4}, {2.0, -0.5, -5.6}, {3.0, -0.5, -6.0}, {3.5, -0.5, -5.5},
					{0.5, -0.1, -0.4}, {2.0, 0.0, -0.4},  {3.0, 0.1, -0.4},  {3.5, 0.3, -0.4},
					{0.5, -0.1, 0.4},  {2.0, 0.0, 0.4},   {3.0, 0.1, 0.4},   {3.5, 0.3, 0.4},
					{0.5, -0.5, 4.4},  {2.0, -0.5, 5.6},  {3.0, -0.5, 6.0},  {3.5, -0.5, 5.5}};
			createEvaluator(&bottomBackWings[0][0], NULL, NULL, &bottom_color[0][0]);

			GLfloat frontVerticalWings[16][3] = {
					{1.0, 1.0, 0.0}, {2.5, 0.9, 0.2},  {3.7, 0.9, 0.2},  {5.3, 0.9, 0.2},
					{1.0, 1.0, 0.0}, {2.5, 1.8, 0.2},  {3.7, 2.4, 0.2},  {5.3, 3.8, 0.2},
					{1.0, 1.0, 0.0}, {2.5, 1.8, -0.2}, {3.7, 2.4, -0.2}, {5.3, 3.8, -0.2},
					{1.0, 1.0, 0.0}, {2.5, 0.9, -0.2}, {3.7, 0.9, -0.2}, {5.3, 0.9, -0.2}};
			createEvaluator(&frontVerticalWings[0][0], NULL, NULL, &VW_color[0][0]);

			GLfloat backVerticalWings[16][3] = {
					{5.3, 1.0, 0.2},  {5.9, 1.0, 0.15},  {6.0, 1.0, 0.1},  {6.4, 1.0, 0.0},
					{5.3, 3.8, 0.2},  {5.9, 3.8, 0.15},  {6.0, 3.5, 0.1},  {6.4, 1.0, 0.0},
					{5.3, 3.8, -0.2}, {5.9, 3.8, -0.15}, {6.0, 3.5, -0.1}, {6.4, 1.0, 0.0},
					{5.3, 1.0, -0.2}, {5.9, 1.0, -0.15}, {6.0, 1.0, -0.1}, {6.4, 1.0, 0.0}};
			createEvaluator(&backVerticalWings[0][0], NULL, NULL, &black[0][0]);

			//boosters
			drawBooster(1, &RGB[0][0]);
			drawBooster(-1, &RGB[0][0]);

			//wheelsBrackets
			drawBracket(-9, -0.1, -0.15, &grey[0][0]);
			drawBracket(0, -0.1, -1.7, &grey[0][0]);
			drawBracket(0, -0.1, 1.4, &grey[0][0]);

			//wheels
			drawWheel(-9.0, -1.75, -0.15, &black[0][0]);
			drawWheel(-9.0, -1.75, 0.35, &black[0][0]);

			drawWheel(0.0, -1.75, -1.7, &black[0][0]);
			drawWheel(0.0, -1.75, -1.2, &black[0][0]);

			drawWheel(0.0, -1.75, 1.4, &black[0][0]);
			drawWheel(0.0, -1.75, 1.9, &black[0][0]);


		}break;
	}
}


LSFprimitive::~LSFprimitive() {
	// TODO Auto-generated destructor stub
}

Terrain::Terrain(string heightmap, string texturemap, string fragmentshader, string vertexshader){
	this->heightmap=heightmap;
	this->texturemap=texturemap;
	this->fragmentshader=fragmentshader;
	this->vertexshader=vertexshader;

	shader=new LSFshader(texturemap, heightmap,vertexshader,fragmentshader);
}

void Terrain::draw(){
	shader->bind();
	glFrontFace(GL_CW);
	GLfloat ctrlpoints[4][3] = {
				{ -5, 0, 5},
				{ 5, 0, 5},
				{-5, 0, -5},
				{ 5, 0, -5}};


	glColor3f(1.0,1.0,1.0);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 2,  0.0, 1.0, 6, 2,  &ctrlpoints[0][0]);
	GLfloat texturespoints[4][2] = {{0.0, 0.0},{1, 0.0},{0.0, 1},{1, 1}};
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &texturespoints[0][0]);
	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	glNormal3f(0,1,0);
	glMapGrid2f(20, 0.0,1, 20, 0.0,1);
	glEvalMesh2(GL_FILL, 0,20, 0,20);

	glFrontFace(GL_CCW);
	shader->unbind();
}
