#include "ObjModel.h"

ObjModel::ObjModel(string filename){
	char * buf=new char[256];
	strcpy(buf,filename.c_str());
	model = glmReadOBJ(buf);
	delete[] buf;
}

void ObjModel::draw(){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	
	glmFacetNormals(model); 
	glmVertexNormals(model, 90.0);

	glmDraw(model,  GLM_SMOOTH | GLM_TEXTURE);
}