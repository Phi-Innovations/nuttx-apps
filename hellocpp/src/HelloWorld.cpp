#include <stdio.h>
#include <string>

#include "HelloWorld.h"

CHelloWorld::CHelloWorld() {
	mSecret = 42;
	printf("Constructor: mSecret=%d\n",mSecret);
}

CHelloWorld::~CHelloWorld() {

}

bool CHelloWorld::HelloWorld(void) {
	printf("HelloWorld: mSecret=%d\n",mSecret);

	std::string frase = "Fraviofii esta aqui";
	printf("Fravio=%s\n",frase.c_str());
	
	if (mSecret == 42) {
		printf("CHelloWorld: HelloWorld: Hello, world!");
		return true;
	}
	else {
		printf("CHelloWorld: HelloWorld: CONSTRUCTION FAILED!\n");
		return false;
	}
}

