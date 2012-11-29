/****************************************************************************
 * Author:      - André Freitas, p.andrefreitas@gmail.com
 * Author:      - Paulo Araújo, paulojmaraujo@gmail.com
 * Copyright:   - 14/10/2012, LAIG, FEUP
 ****************************************************************************/

#ifndef LSFNODE_H_
#define LSFNODE_H_

#include "LSFprimitive.h"

/**
 * LSFnode: a class that is for the nodes in the scenegraph
 */
class LSFnode{
	public:
	LSFnode(){
		animationRef="none";
		isDisplayList=0;
	}
	char *id;
	float transformMatrix[16]; // When the node is parsed, the transformations are put in a stack
	string appearance; // Reference to the map of appearances
	vector<LSFprimitive> childPrimitives; // All the basic primitives
	vector<string> childNoderefs; // All the child nodes
	bool isDisplayList;
	int displayList;
	string animationRef;
};

#endif /* LSFNODE_H_ */
