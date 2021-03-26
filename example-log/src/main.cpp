#include <stdio.h>
#include "Log.h"
#include <nuttx/config.h>
#include <iostream>

extern "C"
{
	int phigw_main(void) {

		Log::print("Log message example");
		
		return 0;
	}
}

