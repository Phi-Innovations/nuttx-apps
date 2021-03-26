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
     * Simple publisher example
     */
    int simplePublisher(void);
    void exitExample(int status, int sockfd, pthread_t *client_daemon);
    static void publishCallback(void** unused, struct mqtt_response_publish *published);
    static void* clientRefresher(void* client);

public:
    PhiGateway();

    int run(void);
};
