#include <stdio.h>
#include "HelloWorld.h"

CHelloWorld::CHelloWorld() {
	mSecret = 42;
	printf("Constructor: mSecret=%d\n",mSecret);
}

CHelloWorld::~CHelloWorld() {

}

bool CHelloWorld::HelloWorld(void) {
	printf("HelloWorld: mSecret=%d\n",mSecret);
	
	if (mSecret == 42) {
		printf("CHelloWorld: HelloWorld: Hello, world!");
		return true;
	}
	else {
		printf("CHelloWorld: HelloWorld: CONSTRUCTION FAILED!\n");
		return false;
	}
}

