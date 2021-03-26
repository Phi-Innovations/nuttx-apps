#pragma once

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <pthread.h>

#include "cdcacm.h"

class PhiGateway {
private:
    struct in_addr addr;
    uint8_t mac[IFHWADDRLEN];

    int setupNetwork(void);

    /*
     * Echo server example
     */
    int startEchoServer(void);

public:
    PhiGateway();

    int run(void);
};
