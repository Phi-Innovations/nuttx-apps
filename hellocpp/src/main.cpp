#include <stdio.h>
#include "HelloWorld.h"

extern "C"
{
	int hello_main(void)
	{
		printf("Inicio de execucao do programa\n");

		CHelloWorld *pHelloWorld = new CHelloWorld();
		pHelloWorld->HelloWorld();

		CHelloWorld helloWorld;
		helloWorld.HelloWorld();

		delete pHelloWorld;
		return 0;
	}
}

