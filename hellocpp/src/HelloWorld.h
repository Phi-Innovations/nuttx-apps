#ifndef HELLOWORLD_H_
#define HELLOWORLD_H_

#include "nuttx/config.h"

class CHelloWorld
{
	public:
		CHelloWorld();
		~CHelloWorld();
		bool HelloWorld(void);
	private:
		int mSecret;
};

#endif
