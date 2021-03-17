#include <stdio.h>
#include "HelloWorld.h"
#include "Log.h"
#include <nuttx/config.h>
#include <iostream>

extern "C"
{
	int phigw_main(void)
	{
		Log::print("PHI: Exemplo em CMake");

		CHelloWorld *pHelloWorld = new CHelloWorld();
		pHelloWorld->HelloWorld();

		CHelloWorld helloWorld;
		helloWorld.HelloWorld();

		delete pHelloWorld;
		return 0;
	}
}

