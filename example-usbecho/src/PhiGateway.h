#pragma once

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <pthread.h>

#include "cdcacm.h"

class PhiGateway {
private:
    /*
     * USB CDB Echo server
     */
    int startUSBEchoServer(void);
    struct cdcacm_state_s g_cdcacm;
    int startUSBDevice(void);
    int stopUSBDevice(void);

public:
    PhiGateway();

    int run(void);
};
