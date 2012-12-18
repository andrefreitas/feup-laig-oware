/****************************************************************************
 * Author:      - André Freitas, p.andrefreitas@gmail.com
 * Author:      - Paulo Araújo, paulojmaraujo@gmail.com
 * Copyright:   - 14/10/2012, LAIG, FEUP
 ****************************************************************************/

#ifndef LSFINTERFACE_H_
#define LSFINTERFACE_H_

#include <iostream>
#include "CGFinterface.h"
#include "LSFscene.h"

/**
 * LSFinterface - a class that implements a CGFinterface
 */
class LSFinterface: public CGFinterface{
	private:
		LSFscene *scene;
		struct globalsData *globals;
		int initialCamera;
		int camerasGroup;
		int polygonalMode;
		GLenum face;
		GLenum mode;
		map<string,LSFlight*> *lights;
		map<string, LSFcamera*> *cameras;

	public:
		LSFinterface(LSFscene *scene);
		~LSFinterface();
		virtual void initGUI();
		virtual void processGUI(GLUI_Control *ctrl);

};

#endif /* LSFINTERFACE_H_ */
