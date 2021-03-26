#include <stdio.h>
#include "PhiGateway.h"
#include <nuttx/config.h>
#include <iostream>

extern "C"
{
	int phigw_main(void) {
		printf("Starting echo server\n");

		PhiGateway *gw = new PhiGateway();

		gw->run();

		delete gw;
		
		return 0;
	}
}

