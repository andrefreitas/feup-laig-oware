#include <iostream>
#include <exception>
#include "CGFapplication.h"
#include "LSFscene.h"
#include "LSFobjects.h"
#include "LSFinterface.h"

using std::cout;
using std::exception;
using namespace std;

int main(int argc, char* argv[]) {

	CGFapplication app = CGFapplication();

	LSFscene *scene = new LSFscene(argv);
	LSFinterface *interface = new LSFinterface(scene);

	try {
		app.init(&argc, argv);

		app.setScene(scene);
		app.setInterface(interface);
		
		app.run();
	}
	catch(GLexception& ex) {
		cout << "Erro: " << ex.what();
		return -1;
	}
	catch(exception& ex) {
		cout << "Erro inesperado: " << ex.what();
		return -1;
	}
	return 0;
}
