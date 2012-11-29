#include "LSFshader.h"
#include <GL/glew.h>

LSFshader::LSFshader(string texturemap, string heightmap, string vertexshader, string fragmentshader){

	init(vertexshader.c_str(),fragmentshader.c_str());
	CGFshader::bind();

	// Initialize parameter in memory
	normScale=2.0;

	// Store Id for the uniform "normScale", new value will be stored on bind()
	scaleLoc = glGetUniformLocation(id(), "normScale");


	this->texturemap=new CGFtexture(texturemap);
	this->heightmap=new CGFtexture(heightmap);

	baseImageLoc = glGetUniformLocation(id(), "baseImage");
	glUniform1i(baseImageLoc, 0);
	secImageLoc = glGetUniformLocation(id(), "secondImage");
	glUniform1i(secImageLoc, 1);
}
 void LSFshader::bind(void){
	CGFshader::bind();

	// update uniforms
	glUniform1f(scaleLoc, normScale);


	glActiveTexture(GL_TEXTURE0);
	texturemap->apply();

	glActiveTexture(GL_TEXTURE1);
	heightmap->apply();

	glActiveTexture(GL_TEXTURE0);

}
