#include "LSFparser.h"

#ifdef __linux__
char* itoa(int val,char *str, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	str=&buf[i+1];
	return &buf[i+1];
	
}
#endif

void exit_(string str, int error = 0) {
	cout << "\n\n\n" << endl;
	cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>   ERROR   <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	cout << ">>                                                                     <<" << endl;
	cout << "   " << str; if (error != 0) cout << " Row " << error; cout << endl;
	cout << ">>                                                                     <<" << endl;
	cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	getchar(); // wait feedback
	exit(-1);
}

LSFparser::LSFparser(char* a) {
	// Load the File
	if (DEBUGMODE)
		cout << "lsfParser(" << a << ");\n";
	lsfFile = new TiXmlDocument(a);

	cout << "XML open: ";
	(lsfFile->LoadFile()) ? cout << "OK" : cout << "ERROR";
	cout << "Erro: " << lsfFile->ErrorDesc() << endl; 
	cout << endl;

	if (lsfFile->LoadFile()) {
		// Fetch main elements sections
		if ((lsfElement = lsfFile->FirstChildElement("lsf")) == NULL)
			exit_("Tag <lsf> is missing or misspelled.");

		if ((globalsElement = lsfElement->FirstChildElement("globals")) == NULL)
			exit_("Tag <globals> is missing or misspelled.");

		if ((camerasElement = lsfElement->FirstChildElement("cameras")) == NULL)
			exit_("Tag <cameras> is missing or misspelled.");

		if ((graphElement = lsfElement->FirstChildElement("graph")) == NULL)
			exit_("Tag <graph> is missing or misspelled.");

		if ((appearancesElement = lsfElement->FirstChildElement("appearances"))
				== NULL)
			exit_("Tag <appearances> is missing or misspelled.");

		if ((lightingsElement = lsfElement->FirstChildElement("lighting"))
				== NULL)
			exit_("Tag <lighting> is missing or misspelled.");

		// Don't check animations because a scene don't need to have them
		animationElement=lsfElement->FirstChildElement("animations");

	} else
		exit_(lsfFile->ErrorDesc(), lsfFile->ErrorRow());
	// Parse
}

void LSFparser::getGlobals(struct globalsData *globals) {
	// Background:
	TiXmlElement *background;
	if ((background = globalsElement->FirstChildElement("background")) == NULL)
		exit_("Tag <background> is missing or misspelled.");

	queryResult =  background->QueryFloatAttribute("r", &globals->background[0]);
	queryResult |= background->QueryFloatAttribute("g", &globals->background[1]);
	queryResult |= background->QueryFloatAttribute("b", &globals->background[2]);
	queryResult |= background->QueryFloatAttribute("a", &globals->background[3]);

	if (queryResult != TIXML_SUCCESS)
		exit_("There is an error in the background values.");

	// Polygon
	TiXmlElement *polygon;
	if ((polygon = globalsElement->FirstChildElement("polygon")) == NULL)
		exit_("Tag <polygon> is missing or misspelled.");

	if ((globals->polygon_mode = polygon->Attribute("mode")) == NULL)
		exit_("polygon mode is missing or misspelled.");

	if ((globals->polygon_shading = polygon->Attribute("shading")) == NULL)
		exit_("polygon shading is missing or misspelled.");

	// Culling
	TiXmlElement *culling;
	if ((culling = globalsElement->FirstChildElement("culling")) == NULL)
		exit_("Tag <culling> is missing or misspelled.");

	if ((globals->culling__frontfaceorder = culling->Attribute("frontfaceorder")) == NULL)
		exit_("culling frontfaceorder is missing or misspelled.");

	if ((globals->culling_cullface = culling->Attribute("cullface")) == NULL)
		exit_("culling cullface is missing or misspelled.");

	if ((culling->QueryBoolAttribute("enabled", &globals->culling_enabled)) != TIXML_SUCCESS)
		exit_("culling enabled is missing or misspelled.");

	if (DEBUGMODE) {
		cout << "\n--- Globals---\n";
		cout << "\tbackground: " << globals->background[0] << ' ' << globals->background[1] << ' '
				<< globals->background[2] << ' ' << globals->background[3] << endl;
		cout << "\tpolygon: " << globals->polygon_mode << ' ' << globals->polygon_shading << endl;
		cout << "\tculling: " << globals->culling__frontfaceorder << ' '
				<< globals->culling_cullface << ' ' << globals->culling_enabled << endl;
	}
}

void LSFparser::getCameras(map<string, LSFcamera*> &cameras) {
	TiXmlElement *node = camerasElement->FirstChildElement();
	int counter = 0;
	const char *initial;

	if (camerasElement->Attribute("initial"))
		initial = camerasElement->Attribute("initial");
	else
		exit_("camera initial attribute is missing or misspelled.");

	if (DEBUGMODE)
		cout << "\n--- Cameras: " << initial << " ---";

	// Loop trough cameras
	while (node) {
		counter++;

		if ((string) node->Value() != "perspective" && (string) node->Value() != "ortho")
			exit_("Tag <" + (string) node->Value() + "> not valid");

		LSFcamera *camera = new LSFcamera();
		camera->view.assign(node->Value());

		if (strcmp(node->Value(), "ortho") == 0) {
			if (node->Attribute("id"))
				camera->id = node->Attribute("id");

			if (camera->id.empty())
				exit_("camera id attribute is missing or misspelled.");

			queryResult =  node->QueryFloatAttribute("near", &camera->_near);
			queryResult |= node->QueryFloatAttribute("far", &camera->_far);
			queryResult |= node->QueryFloatAttribute("left", &camera->left);
			queryResult |= node->QueryFloatAttribute("right", &camera->right);
			queryResult |= node->QueryFloatAttribute("top", &camera->top);
			queryResult |= node->QueryFloatAttribute("bottom", &camera->bottom);

			if (queryResult != TIXML_SUCCESS)
				exit_("There is an error in ortho \"" + camera->id + "\" camera values");

			if (DEBUGMODE) {
				float attributes[] = { camera->_near, camera->_far, camera->left,
								       camera->right, camera->top, camera->bottom };
				cout << "\n\tOrtho: " << camera->id << " ";

				for (int unsigned i = 0; i < 6; i++)
					cout << attributes[i] << " ";

				cout << endl;
			}
		} else {
			if (node->Attribute("id"))
				camera->id = node->Attribute("id");

			if (camera->id.empty())
				exit_("camera id attribut is missing or misspelled.");

			queryResult = node->QueryFloatAttribute("near", &camera->_near);
			queryResult |= node->QueryFloatAttribute("far", &camera->_far);
			queryResult |= node->QueryFloatAttribute("angle", &camera->angle);

			// From
			TiXmlElement *from, *to;
			if ((from = node->FirstChildElement("from")) == NULL)
				exit_("Tag <from> is missing or misspelled.");

			queryResult |= from->QueryFloatAttribute("x", &camera->fromX);
			queryResult |= from->QueryFloatAttribute("y", &camera->fromY);
			queryResult |= from->QueryFloatAttribute("z", &camera->fromZ);

			// To
			if ((to = node->FirstChildElement("to")) == NULL)
				exit_("Tag <to> is missing or misspelled.");

			queryResult |= to->QueryFloatAttribute("x", &camera->toX);
			queryResult |= to->QueryFloatAttribute("y", &camera->toY);
			queryResult |= to->QueryFloatAttribute("z", &camera->toZ);

			if (queryResult != TIXML_SUCCESS)
				exit_("There is an error in perspective \"" + camera->id + "\" camera values");

			if (DEBUGMODE) {
				float attributes[] = { camera->_near, camera->_far, camera->angle };
				cout << "\n\tPerspective: " << camera->id << " ";

				for (int unsigned i = 0; i < 3; i++)
					cout << attributes[i] << " ";

				cout << endl;
				cout << "\t\t\tFrom: " << camera->fromX << " " << camera->fromY << " " << camera->fromZ << endl;
				cout << "\t\t\tTo: " << camera->toX << " " << camera->toY << " " << camera->toZ << endl;
			}
		}

		if (camera->id == initial)
			camera->initial = true;
		else
			camera->initial = false;

		cameras[camera->id] = camera;

		// -->
		node = node->NextSiblingElement();
	}
}

void LSFparser::getNodes(map<string, LSFnode*> &nodes, string &rootNode) {
	const char * rootid = graphElement->Attribute("rootid");
	if (graphElement->Attribute("rootid"))
		rootNode.assign(rootid);
	else
		exit_("graph rootid is missing or misspelled.");

	if (DEBUGMODE)
		cout << "\n--- Graph: " << rootid << " ---" << endl;

	TiXmlElement *node = graphElement->FirstChildElement();

	while (node) {
		LSFnode *pnode = new LSFnode();
		pnode->id = new char[100];

		if (node->Attribute("id"))
			strcpy(pnode->id, node->Attribute("id")); // save in the node
		else
			exit_("graph " + (string) node->Value() + " id is missing or misspelled.");

		if (DEBUGMODE)
			cout << "\tNode: " << node->Attribute("id") << endl;

		// Animation ID
		TiXmlElement *animation=node->FirstChildElement("animationref");
		if(animation!=0) pnode->animationRef=string(animation->Attribute("id"));

		// (1) Transforms
		TiXmlElement *transforms;
		if ((transforms = node->FirstChildElement("transforms")) == NULL)
			exit_("Tag <transforms> at node " + (string) pnode->id + " is missing or misspelled.");

		TiXmlElement *transform = transforms->FirstChildElement();

		if (DEBUGMODE)
			cout << "\tTransforms:" << endl;

		// Compute transforms
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		stack<Transform> transfs;
		// --->
		int existingTransforms = 0;
		int existingValidTransforms = 0;

		while (transform) {
			existingTransforms++;
			float x, y, z, angle;
			char axis;
			const char* transVal = transform->Value();

			if (strcmp(transVal, "translate") == 0) {
				existingValidTransforms++;
				queryResult =  transform->QueryFloatAttribute("x", &x);
				queryResult |= transform->QueryFloatAttribute("y", &y);
				queryResult |= transform->QueryFloatAttribute("z", &z);

				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in translate values at node " + (string) pnode->id + ".");

				if (DEBUGMODE)
					cout << "\t\tTranslate: " << x << " " << y << " " << z << endl;

				glTranslatef(x, y, z);

			} else if (strcmp(transVal, "rotate") == 0) {
				existingValidTransforms++;
				if (transform->QueryFloatAttribute("angle", &angle) != TIXML_SUCCESS)
					exit_("There is an error in rotate values at node " + (string) pnode->id + ".");

				axis = *transform->Attribute("axis");

				if (DEBUGMODE)
					cout << "\t\tRotate: " << angle << " " << axis << endl;

				x = 0;
				y = 0;
				z = 0;
				if (axis == 'x')
					x = 1;
				else if (axis == 'y')
					y = 1;
				else if (axis == 'z')
					z = 1;
				else
					exit_("There is an error in rotate values at node " + (string) pnode->id + ".");

				glRotatef(angle, x, y, z);

			} else if (strcmp(transVal, "scale") == 0) {
				existingValidTransforms++;

				queryResult =  transform->QueryFloatAttribute("x", &x);
				queryResult |= transform->QueryFloatAttribute("y", &y);
				queryResult |= transform->QueryFloatAttribute("z", &z);

				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in scale values at node " + (string) pnode->id + ".");

				if (DEBUGMODE)
					cout << "\t\tScale: " << x << " " << y << " " << z << endl;

				glScalef(x, y, z);
			}

			// -->
			transform = transform->NextSiblingElement();
		}

		char tbuffer[33];
		itoa((existingTransforms - existingValidTransforms), tbuffer, 30);

		if (existingTransforms != existingValidTransforms)
			exit_("Exists " + (string) tbuffer + " invalid transform(s) at node " + (string) pnode->id + ".");

		glGetFloatv(GL_MODELVIEW_MATRIX, pnode->transformMatrix);
		glPopMatrix();

		// (2) Appearance

		TiXmlElement *appearanceref;
		if ((appearanceref = node->FirstChildElement("appearanceref")) == NULL)
			exit_("Tag <appearanceref> at node " + (string) pnode->id + " is missing or misspelled.");

		if (appearanceref->Attribute("id"))
			pnode->appearance = (string) appearanceref->Attribute("id");
		else
			exit_("appearanceref id at node " + (string) pnode->id + " is missing or misspelled.");

		if (DEBUGMODE)
			cout << "\tAppearance: " << pnode->appearance << endl;

		// (3) Children
		if (DEBUGMODE)
			cout << "\tChildren: " << endl;

		TiXmlElement *children;
		if ((children = node->FirstChildElement("children")) == NULL)
			exit_("Tag <children> at node " + (string) pnode->id + " is missing or misspelled.");

		TiXmlElement *child = children->FirstChildElement();
		if (child == NULL)
			exit_("Tag <children> at node " + (string) pnode->id + " is empty");

		int existingChilds = 0;
		int existingValidChilds = 0;

		while (child) {
			existingChilds++;
			const char* childVal = child->Value();

			map<const char*, float> attr;
			// -->

			if (strcmp(childVal, "rectangle") == 0) {
				existingValidChilds++;
				float x1, x2, y1, y2;

				queryResult =  child->QueryFloatAttribute("x1", &x1);
				queryResult |= child->QueryFloatAttribute("x2", &x2);
				queryResult |= child->QueryFloatAttribute("y1", &y1);
				queryResult |= child->QueryFloatAttribute("y2", &y2);

				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in rectangle values at node " + (string) pnode->id + ".");

				if (DEBUGMODE)
					cout << "\t\trectangle: " << x1 << " " << y1 << " " << x2 << " " << y2 << endl;

				pnode->childPrimitives.push_back(new LSFRectangle(x1, x2, y1, y2));

			} else if(strcmp(childVal, "box") == 0){
				existingValidChilds++;
				float x1, x2, y1, y2, z1, z2;

				queryResult =  child->QueryFloatAttribute("x1", &x1);
				queryResult |= child->QueryFloatAttribute("x2", &x2);
				queryResult |= child->QueryFloatAttribute("y1", &y1);
				queryResult |= child->QueryFloatAttribute("y2", &y2);
				queryResult |= child->QueryFloatAttribute("z1", &z1);
				queryResult |= child->QueryFloatAttribute("z2", &z2);

				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in box values at node " + (string) pnode->id + ".");

				if (DEBUGMODE)
					cout << "\t\tbox: " << x1 << " " << y1 << " " << z1 << " " << x2 << " " << y2 << " " << z2<< endl;

				pnode->childPrimitives.push_back(new LSFBox(x1, x2, y1, y2, z1, z2));

			} else if (strcmp(childVal, "triangle") == 0) {
				existingValidChilds++;
				float x1, x2, x3, y1, y2, y3, z1, z2, z3;

				queryResult =  child->QueryFloatAttribute("x1", &x1);
				queryResult |= child->QueryFloatAttribute("x2", &x2);
				queryResult |= child->QueryFloatAttribute("x3", &x3);
				queryResult |= child->QueryFloatAttribute("y1", &y1);
				queryResult |= child->QueryFloatAttribute("y2", &y2);
				queryResult |= child->QueryFloatAttribute("y3", &y3);
				queryResult |= child->QueryFloatAttribute("z1", &z1);
				queryResult |= child->QueryFloatAttribute("z2", &z2);
				queryResult |= child->QueryFloatAttribute("z3", &z3);

				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in triangle values at node " + (string) pnode->id + ".");

				if (DEBUGMODE)
					cout << "\t\ttriangle " << x1 << " " << y1 << " " << z1 << " "
							<< x2 << " " << y2 << " " << z2 << " " << x3 << " "
							<< y3 << " " << z3 << endl;

				// Compute the normal
				LSFvertex v1(x1, y1, z1);
				LSFvertex v2(x2, y2, z2);
				LSFvertex v3(x3, y3, z3);
				vector<LSFvertex> vertexs;
				vertexs.push_back(v3); // Need to be in this order
				vertexs.push_back(v2);
				vertexs.push_back(v1);
				LSFvertex normal = computeNormalNewel(vertexs);

				// UV coords
				vector<LSFvertex> uvCoords;
				uvCoords.push_back(LSFvertex(x1, y1, z1));
				uvCoords.push_back(LSFvertex(x2, y2, z2));
				uvCoords.push_back(LSFvertex(x3, y3, z3));
				uvCoords = computeTriangleUV(uvCoords);

				pnode->childPrimitives.push_back(new LSFTriangle(x1, x2, x3, y1, y2, y3, z1, z2, z3, normal, uvCoords));

			} else if (strcmp(childVal, "cylinder") == 0) {
				existingValidChilds++;
				float base, top, height;
				int slices, stacks;

				queryResult =  child->QueryFloatAttribute("base", &base);
				queryResult |= child->QueryFloatAttribute("top", &top);
				queryResult |= child->QueryFloatAttribute("height", &height);
				queryResult |= child->QueryIntAttribute("slices", &slices);
				queryResult |= child->QueryIntAttribute("stacks", &stacks);

				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in cylinder values at node " + (string) pnode->id + ".");

				if (DEBUGMODE)
					cout << "\t\tcylinder " << base << " " << top << " " << height
							<< " " << slices << " " << stacks << endl;

				pnode->childPrimitives.push_back(new LSFCylinder(base, top, height, slices, stacks));

			} else if (strcmp(childVal, "sphere") == 0) {
				existingValidChilds++;
				float radius;
				int slices, stacks;

				queryResult =  child->QueryFloatAttribute("radius", &radius);
				queryResult |= child->QueryIntAttribute("slices", &slices);
				queryResult |= child->QueryIntAttribute("stacks", &stacks);

				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in sphere values at node " + (string) pnode->id + ".");

				if (DEBUGMODE)
					cout << "\t\tsphere " << radius << " " << slices << " " << stacks << endl;

				pnode->childPrimitives.push_back(new LSFSphere(radius, slices, stacks));

			} else if (strcmp(childVal, "torus") == 0) {
				existingValidChilds++;
				float inner, outer;
				int slices, loops;

				queryResult =  child->QueryFloatAttribute("inner", &inner);
				queryResult |= child->QueryFloatAttribute("outer", &outer);
				queryResult |= child->QueryIntAttribute("slices", &slices);
				queryResult |= child->QueryIntAttribute("loops", &loops);

				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in triangle values at node " + (string) pnode->id + ".");

				if (DEBUGMODE)
					cout << "\t\ttorus " << inner << " " << outer << " " << slices << " " << loops << endl;

				pnode->childPrimitives.push_back(new LSFTorus(inner, outer, slices, loops));

			} else if (strcmp(childVal, "noderef") == 0) {
				existingValidChilds++;
				const char *idRef = child->Attribute("id");
				string st(idRef);
				pnode->childNoderefs.push_back(st);

				if (DEBUGMODE)
					cout << "\t\tnoderef " << st << endl;
			}
			else if (strcmp(childVal, "model") == 0) {
				existingValidChilds++;
				const char *filename=child->Attribute("src");
				char *buf =new char[256];
				strcpy(buf,"../models/");
				strcat(buf,filename);
				cout << "Path: " << buf << endl;
				pnode->childPrimitives.push_back(new LSFModel(buf));
			}

			// -->
			child = child->NextSiblingElement();
		}

		char buffer[33];
		itoa((existingChilds - existingValidChilds), buffer, 30);
		if (existingChilds != existingValidChilds)
			exit_("Exists " + (string) buffer + " invalid child(s) at node " + (string) pnode->id + ".");

		// -->
		nodes[(string) pnode->id] = pnode; // Add this node
		node = node->NextSiblingElement();
		if (DEBUGMODE)
			cout << "\n\t-----\n";
	}

	bool rootidOK = false;
	map<string, LSFnode*>::iterator it;
	for (it = nodes.begin(); it != nodes.end(); it++) {
		if ((*it).first == rootid) {
			rootidOK = true;
			break;
		}
	}

	if (!rootidOK)
		exit_("graph rootid is missing or misspelled.");
}

void LSFparser::getAppearances(map<string, LSFappearance*> &appearances) {
	TiXmlElement *node = appearancesElement->FirstChildElement();
	int counter = 0;
	float emissive_vec[4], ambient_vec[4], diffuse_vec[4], specular_vec[4];

	float shininess_value, texture_length_s, texture_length_t;
	if (DEBUGMODE)
		cout << "\n--- Aparencias  ---" << endl;
	// Loop trough appearances

	while (node) {
		counter++;

		int existingValidAppearanceElements = 0;

		TiXmlElement *emissive, *ambient, *diffuse, *specular, *shininess, *texture;

		emissive = node->FirstChildElement("emissive");
		if (emissive != NULL)
			if (strcmp(emissive->ValueTStr().c_str(), "emissive") == 0) {
				existingValidAppearanceElements++;
				queryResult =  emissive->QueryFloatAttribute("r", &emissive_vec[0]);
				queryResult |= emissive->QueryFloatAttribute("g", &emissive_vec[1]);
				queryResult |= emissive->QueryFloatAttribute("b", &emissive_vec[2]);
				queryResult |= emissive->QueryFloatAttribute("a", &emissive_vec[3]);
				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in emissive values at appearance " + (string) node->Attribute("id") + ".");
			}

		ambient = node->FirstChildElement("ambient");
		if (ambient != NULL)
			if (strcmp(ambient->ValueTStr().c_str(), "ambient") == 0) {
				existingValidAppearanceElements++;
				queryResult =  ambient->QueryFloatAttribute("r", &ambient_vec[0]);
				queryResult |= ambient->QueryFloatAttribute("g", &ambient_vec[1]);
				queryResult |= ambient->QueryFloatAttribute("b", &ambient_vec[2]);
				queryResult |= ambient->QueryFloatAttribute("a", &ambient_vec[3]);
				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in ambient values at appearance " + (string) node->Attribute("id") + ".");
			}

		diffuse = node->FirstChildElement("diffuse");
		if (diffuse != NULL)
			if (strcmp(diffuse->ValueTStr().c_str(), "diffuse") == 0) {
				existingValidAppearanceElements++;
				queryResult =  diffuse->QueryFloatAttribute("r", &diffuse_vec[0]);
				queryResult |= diffuse->QueryFloatAttribute("g", &diffuse_vec[1]);
				queryResult |= diffuse->QueryFloatAttribute("b", &diffuse_vec[2]);
				queryResult |= diffuse->QueryFloatAttribute("a", &diffuse_vec[3]);
				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in diffuse values at appearance " + (string) node->Attribute("id") + ".");
			}

		specular = node->FirstChildElement("specular");
		if (specular != NULL)
			if (strcmp(specular->ValueTStr().c_str(), "specular") == 0) {
				existingValidAppearanceElements++;
				queryResult =  specular->QueryFloatAttribute("r", &specular_vec[0]);
				queryResult |= specular->QueryFloatAttribute("g", &specular_vec[1]);
				queryResult |= specular->QueryFloatAttribute("b", &specular_vec[2]);
				queryResult |= specular->QueryFloatAttribute("a", &specular_vec[3]);
				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in specular values at appearance " + (string) node->Attribute("id") + ".");
			}

		shininess = node->FirstChildElement("shininess");
		if (shininess != NULL)
			if (strcmp(shininess->ValueTStr().c_str(), "shininess") == 0) {
				existingValidAppearanceElements++;
				if (shininess->QueryFloatAttribute("value", &shininess_value) != TIXML_SUCCESS)
					exit_("There is an error in shininess value at appearance " + (string) node->Attribute("id") + ".");
			}

		char abuffer[33];
		itoa((5 - existingValidAppearanceElements), abuffer, 30);
		if (5 != existingValidAppearanceElements)
			exit_("Exists " + (string) abuffer + " invalid appearance element(s) at node "
							+ (string) node->Attribute("id") + ".");

		texture = node->FirstChildElement("texture");
		if (texture != NULL)
			if (strcmp(texture->ValueTStr().c_str(), "texture") == 0) {
				existingValidAppearanceElements++;
				queryResult =  texture->QueryFloatAttribute("length_s", &texture_length_s);
				queryResult |= texture->QueryFloatAttribute("length_t", &texture_length_t);
				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in texture values at appearance " + (string) node->Attribute("id") + ".");
			}

		if (DEBUGMODE) {
			cout << "\n\tAparencia: " << node->Attribute("id") << " " << endl;
			if (emissive != NULL) {
				cout << "emissive ";
				for (int i = 0; i < 4; i++)
					cout << emissive_vec[i] << " ";
				cout << endl;
			}
			if (ambient != NULL) {
				cout << "ambient ";
				for (int i = 0; i < 4; i++)
					cout << ambient_vec[i] << " ";
				cout << endl;
			}
			if (diffuse != NULL) {
				cout << "diffuse ";
				for (int i = 0; i < 4; i++)
					cout << diffuse_vec[i] << " ";
				cout << endl;
			}
			if (specular != NULL) {
				cout << "specular ";
				for (int i = 0; i < 4; i++)
					cout << specular_vec[i] << " ";
				cout << endl;
			}
			if (shininess != NULL)
				cout << "shininess value=" << shininess_value << endl;
			if (texture != NULL)
				cout << "texture file=" << texture->Attribute("file")
						<< " length_s=" << texture_length_s << " length_t="
						<< texture_length_t << endl;

			cout << endl;
		}
		// Add to the map
		CGFappearance* pappearance;
		LSFappearance *pLSFappearance = new LSFappearance();
		pappearance = new CGFappearance(ambient_vec, diffuse_vec, specular_vec, shininess_value);
		if (texture != NULL) {
			string t = "../textures/";
			t.append(texture->Attribute("file"));
			CGFtexture *text = new CGFtexture(t);
			//text->setSize(texture_length_s, texture_length_t);
			pappearance->setTexture(text);
			pappearance->setTextureWrap(GL_REPEAT, GL_REPEAT);
			if (DEBUGMODE)
				cout << "\nSettexture: " << texture->Attribute("file") << endl;
		}
		// -->
		string auxId;
		auxId.assign(node->Attribute("id"));
		// Attribute to LSFAppearance
		pLSFappearance->appearance = pappearance;
		pLSFappearance->emissive[0] = emissive_vec[0];
		pLSFappearance->emissive[1] = emissive_vec[1];
		pLSFappearance->emissive[2] = emissive_vec[2];
		pLSFappearance->emissive[3] = emissive_vec[3];
		pLSFappearance->id = auxId;
		if (texture != NULL) {
			pLSFappearance->length_s = texture_length_s;
			pLSFappearance->length_t = texture_length_t;
			pLSFappearance->haveTexture = true;
		} else {
			pLSFappearance->haveTexture = false;
		}
		appearances[auxId] = pLSFappearance;
		node = node->NextSiblingElement();
	}

}

void LSFparser::getLights(map<string, LSFlight*>&lights, bool &enabled, bool &local, bool &doublesided, float *sceneAmbient) {
	int counter = 0;
	const char *light_id;
	int lightsId[] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };

	queryResult =  lightingsElement->QueryBoolAttribute("local", &local);
	queryResult |= lightingsElement->QueryBoolAttribute("enabled", &enabled);
	queryResult |= lightingsElement->QueryBoolAttribute("doublesided", &doublesided);
	if (queryResult != TIXML_SUCCESS)
		exit_("There is an error in ambient light options.");

	if (DEBUGMODE) {
		cout << "\n--- Luzes  ---" << endl;
		if (doublesided)
			cout << "doubleSided = true" << endl;
		if (local)
			cout << "local = true" << endl;
		if (enabled)
			cout << "enabled = true" << endl;
	}

	TiXmlElement *scene_ambient = lightingsElement->FirstChildElement("ambient");

	//ambiente global
	if (scene_ambient != NULL) {
		queryResult =  scene_ambient->QueryFloatAttribute("r", &sceneAmbient[0]);
		queryResult |= scene_ambient->QueryFloatAttribute("g", &sceneAmbient[1]);
		queryResult |= scene_ambient->QueryFloatAttribute("b", &sceneAmbient[2]);
		queryResult |= scene_ambient->QueryFloatAttribute("a", &sceneAmbient[3]);
		if (queryResult != TIXML_SUCCESS)
			exit_("There is an error in ambient rgba values.");
	} else
		exit_("Tag <ambient> at global ambient is missing or misspelled.");

	if (DEBUGMODE && scene_ambient != NULL)
		cout << "scene ambient r=" << sceneAmbient[0] << " g=" << sceneAmbient[1] << " b="
		     << sceneAmbient[2] << " a=" << sceneAmbient[3] << endl;

	TiXmlElement *node_ = lightingsElement->FirstChildElement("lights");

	if (node_ == NULL)
		exit_("Tag <lights> is missing or misspelled.");

	TiXmlElement *node = node_->FirstChildElement("light");

	// Loop lights
	while (node) {
		CGFlight *plight;
		counter++;
		// Detect if there are more than 8 lights
		if (counter > 8)
			exit_("YOU HAVE MORE THAN 8 LIGHTS!");

		// new light
		LSFlight *pLSFlight = new LSFlight;

		float * positionV = new float[4];
		float * directionV = new float[4];
		float * ambientV = new float[4]; //ambiente de cada luz
		float * diffuseV = new float[4];
		float * specularV = new float[4];

		int existingValidLightElements = 0;

//		light_id = node->Attribute("id");

		char lbuffer[33];
		itoa(counter, lbuffer, 30);
		if ((node->Attribute("id")) != NULL)
			pLSFlight->id = light_id = node->Attribute("id");
		else
			exit_("There is an error in id value at light " + (string) lbuffer + ".");

		if (node->QueryBoolAttribute("enabled", &pLSFlight->enabled) != TIXML_SUCCESS)
			exit_("There is an error in enabled value at light " + (string) node->Attribute("id") + ".");

		TiXmlElement *location, *ambient, *diffuse, *specular, *spot;

		location = node->FirstChildElement("location");
		if (location != NULL)
			if (strcmp(location->ValueTStr().c_str(), "location") == 0) {
				existingValidLightElements++;
				queryResult =  location->QueryFloatAttribute("x", &positionV[0]);
				queryResult |= location->QueryFloatAttribute("y", &positionV[1]);
				queryResult |= location->QueryFloatAttribute("z", &positionV[2]);
				positionV[3] = 1;
				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in location values at light " + (string) lbuffer + ".");
			}

		ambient = node->FirstChildElement("ambient");
		if (ambient != NULL)
			if (strcmp(ambient->ValueTStr().c_str(), "ambient") == 0) {
				existingValidLightElements++;
				queryResult =  ambient->QueryFloatAttribute("r", &ambientV[0]);
				queryResult |= ambient->QueryFloatAttribute("g", &ambientV[1]);
				queryResult |= ambient->QueryFloatAttribute("b", &ambientV[2]);
				queryResult |= ambient->QueryFloatAttribute("a", &ambientV[3]);
				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in ambient values at light " + (string) lbuffer + ".");
			}

		diffuse = node->FirstChildElement("diffuse");
		if (diffuse != NULL)
			if (strcmp(diffuse->ValueTStr().c_str(), "diffuse") == 0) {
				existingValidLightElements++;
				queryResult =  diffuse->QueryFloatAttribute("r", &diffuseV[0]);
				queryResult |= diffuse->QueryFloatAttribute("g", &diffuseV[1]);
				queryResult |= diffuse->QueryFloatAttribute("b", &diffuseV[2]);
				queryResult |= diffuse->QueryFloatAttribute("a", &diffuseV[3]);
				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in diffuse values at light " + (string) lbuffer + ".");
			}

		specular = node->FirstChildElement("specular");
		if (specular != NULL)
			if (strcmp(specular->ValueTStr().c_str(), "specular") == 0) {
				existingValidLightElements++;
				queryResult =  specular->QueryFloatAttribute("r", &specularV[0]);
				queryResult |= specular->QueryFloatAttribute("g", &specularV[1]);
				queryResult |= specular->QueryFloatAttribute("b", &specularV[2]);
				queryResult |= specular->QueryFloatAttribute("a", &specularV[3]);
				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in specular values at light " + (string) lbuffer + ".");
			}

		char lebuffer[33];
		itoa((4 - existingValidLightElements), lebuffer, 30);
		if (4 != existingValidLightElements)
			exit_("Exists " + (string) lebuffer + " invalid light element(s) at light " + (string) lbuffer + ".");

		spot = node->FirstChildElement("spot");
		if (spot != NULL) {
			if (strcmp(spot->ValueTStr().c_str(), "spot") == 0) {
				queryResult =  spot->QueryFloatAttribute("angle", &pLSFlight->angle);
				queryResult |= spot->QueryFloatAttribute("exponent", &pLSFlight->spotExponent);
				queryResult |= spot->QueryFloatAttribute("dirx", &directionV[0]);
				queryResult |= spot->QueryFloatAttribute("diry", &directionV[1]);
				queryResult |= spot->QueryFloatAttribute("dirz", &directionV[2]);
				directionV[3] = 1;
				if (queryResult != TIXML_SUCCESS)
					exit_("There is an error in spot values at light " + (string) lbuffer + ".");
			}
			pLSFlight->isspotLight = true;
		} else {
			directionV = NULL;
			pLSFlight->isspotLight = false;
		}

		// Add light to the vector
		// Create
		plight = new CGFlight(lightsId[counter - 1], positionV);
		// Angle for spotlight
		if (spot != NULL)
			plight->setAngle(pLSFlight->angle);
		// Ambient
		plight->setAmbient(ambientV);
		// Diffuse
		plight->setDiffuse(diffuseV);
		// Specular
		plight->setSpecular(specularV);

		if (spot != NULL) {
			glLightf(lightsId[counter - 1], GL_SPOT_EXPONENT, pLSFlight->spotExponent);
			glLightf(lightsId[counter - 1], GL_SPOT_CUTOFF, pLSFlight->angle);
			glLightfv(lightsId[counter - 1], GL_SPOT_DIRECTION, directionV);
		} else {
			pLSFlight->spotExponent = -1; // Maybe usefull later
		}

		pLSFlight->light = plight;

		lights[light_id] = pLSFlight;

		if (DEBUGMODE) {
			cout << "\n\tLuz: " << light_id << "  enabled=" << pLSFlight->enabled << endl;
			if (location != NULL)
				cout << "location x=" << positionV[0] << " y=" << positionV[1] << " z=" << positionV[2] << endl;
			if (ambient != NULL)
				cout << "ambient  r=" << ambientV[0] << " g=" << ambientV[1]
				     << " b=" << ambientV[2] << " a=" << ambientV[3] << endl;
			if (diffuse != NULL)
				cout << "diffuse  r=" << diffuseV[0] << " g=" << diffuseV[1]
				     << " b=" << diffuseV[2] << " a=" << diffuseV[3] << endl;
			if (specular != NULL)
				cout << "specular r=" << specularV[0] << " g=" << specularV[1]
				     << " b=" << specularV[2] << " a=" << specularV[4] << endl;
			if (spot != NULL) {
				cout << "spot angle=" << pLSFlight->angle << " exponent="
					 << pLSFlight->spotExponent;
				cout << " dirx=" << directionV[0] << " diry=" << directionV[1]
					 << " dirz=" << directionV[2] << endl;
			}
			cout << endl;
		}

		node = node->NextSiblingElement();
	}
}

void LSFparser::getAnimations(map<string,LSFanimation*> &animations){
	if(animationElement==0) {
		cout << "Nada de Animacoes" << endl;
		return; // nothing to do here
	}

	TiXmlElement *node = animationElement->FirstChildElement();
	const char *animationID;
	float animationSpan;
	LSFanimation *animation;
	LSFvertex point;

	cout << "Animations:\n";
	// For every animation
	while(node){
		animationID=node->Attribute("id");
		queryResult = node->QueryFloatAttribute("span",&animationSpan);
		if (queryResult != TIXML_SUCCESS)
			exit_("There is an error in span value at animation " + (string) node->Attribute("id") + ".");

		cout << "-> ID " << animationID << " Span " << animationSpan << endl;

		// For every control point
		TiXmlElement *controlpoint=node->FirstChildElement();
		vector<LSFvertex> cps;
		while(controlpoint){
			queryResult =  controlpoint->QueryDoubleAttribute("xx",&point.x);
			queryResult |= controlpoint->QueryDoubleAttribute("yy",&point.y);
			queryResult |= controlpoint->QueryDoubleAttribute("zz",&point.z);
			if (queryResult != TIXML_SUCCESS)
				exit_("There is an error in controlpoints values at animation " + (string) node->Attribute("id") + ".");

			cout << "--> Control Point "; point.print(); cout << endl;
			cps.push_back(point);
			// -->
			controlpoint = controlpoint->NextSiblingElement();
		}

		// Create an animation
		animation=new LSFanimation(cps,animationSpan);
		animations[string(animationID)]=animation;
		// -->
		node = node->NextSiblingElement();
	}
}
