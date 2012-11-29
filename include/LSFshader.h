
#include "CGFshader.h"
#include "CGFtexture.h"
#ifndef LSFSHADERS_H_
#define LSFSHADERS_H_

class LSFshader: public CGFshader{
protected:
	CGFtexture *texturemap;
	CGFtexture *heightmap;
	int baseImageLoc;
	int secImageLoc;
	float normScale;
	int scaleLoc;
public:
	LSFshader(string texturemap, string heightmap, string vertexshader, string fragmentshader);

	~LSFshader(void){};
	virtual void bind(void);
};
#endif /* LSFSHADERS_H_ */
