#pragma once

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <pthread.h>

class PhiGateway {
private:
    /*
     * Libmodbus example
     */
    int startModbusMaster(void);

public:
    PhiGateway() { };

    int run(void);
};
