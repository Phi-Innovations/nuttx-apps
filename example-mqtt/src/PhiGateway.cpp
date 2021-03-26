#include "PhiGateway.h"

#include <debug.h>
#include <errno.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <nuttx/net/arp.h>
#include "netutils/netlib.h"
#include "netutils/dhcpc.h"

#include "mqtt-c/mqtt.h"
#include "mqtt-c/templates/posix_sockets.h"

PhiGateway::PhiGateway() {
    setupNetwork();    
}

int PhiGateway::setupNetwork(void) {
    struct dhcpc_state ds;
    void *handle;

    mac[0] = 0x00;
    mac[1] = 0xe0;
    mac[2] = 0xde;
    mac[3] = 0xad;
    mac[4] = 0xbe;
    mac[5] = 0xef;
    netlib_setmacaddr("eth0", mac);

    printf("Setup network: IP\n");

    /* Set up our host address */
    addr.s_addr = 0;
    netlib_set_ipv4addr("eth0", &addr);

    /* Set up the default router address */
    printf("Setup network: GATEWAY\n");
    addr.s_addr = HTONL(0xc0a80201);
    netlib_set_dripv4addr("eth0", &addr);

    /* Setup the subnet mask */
    printf("Setup netmask: IP\n");
    addr.s_addr = HTONL(0xffffff00);
    netlib_set_ipv4netmask("eth0", &addr);

    /* 
     * New versions of netlib_set_ipvXaddr will not bring the network up,
     * So ensure the network is really up at this point.
     */

    netlib_ifup("eth0");

    printf("Ethernet initialized\n");

    /* Get the MAC address of the NIC */

    netlib_getmacaddr("eth0", mac);

    /* Set up the DHCPC modules */

    handle = dhcpc_open("eth0", &mac, IFHWADDRLEN);

    /* 
     * Get an IP address.  Note:  there is no logic here for renewing the address in this
     * example.  The address should be renewed in ds.lease_time/2 seconds.
     */

    if (!handle) {
        printf("Problem dhcp_open()\n");
        return ERROR;
    }

    if (dhcpc_request(handle, &ds) != OK) {
        printf("Problem dhcp_request()\n");
        return ERROR;
    }

    netlib_set_ipv4addr("eth0", &ds.ipaddr);

    if (ds.netmask.s_addr != 0) {
        netlib_set_ipv4netmask("eth0", &ds.netmask);
    }

    if (ds.default_router.s_addr != 0) {
        netlib_set_dripv4addr("eth0", &ds.default_router);
    }

    if (ds.dnsaddr.s_addr != 0) {
        netlib_set_ipv4dnsaddr(&ds.dnsaddr);
    }

    dhcpc_close(handle);
    printf("IP: %s\n", inet_ntoa(ds.ipaddr));

    return OK;
}

int PhiGateway::simplePublisher(void) {
    const char* brokerAddr;
    const char* port;
    const char* topic;

    printf("Starting simple publisher\n");

    /* get address (argv[1] if present) */
    // brokerAddr = "test.mosquitto.org";
    brokerAddr = "54.191.223.33";

    /* get port number (argv[2] if present) */
    port = "1883";

    /* get the topic name to publish */
    topic = "datetime";

    printf("Opening socket\n");
    /* open the non-blocking TCP socket (connecting to the broker) */
    int sockfd = open_nb_socket(brokerAddr, port);

    if (sockfd == -1) {
        perror("Failed to open socket: ");
        exitExample(EXIT_FAILURE, sockfd, NULL);
    }

    printf("Initializing MQTT\n");
    /* setup a client */
    struct mqtt_client client;
    uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
    uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */
    mqtt_init(&client, sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publishCallback);
    /* Create an anonymous session */
    const char* client_id = NULL;
    /* Ensure we have a clean session */
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    printf("Connecting to the broker\n");
    /* Send connection request to the broker. */
    mqtt_connect(&client, client_id, NULL, NULL, 0, NULL, NULL, connect_flags, 400);

    /* check that we don't have any errors */
    if (client.error != MQTT_OK) {
        printf("error: %s\n", mqtt_error_str(client.error));
        exitExample(EXIT_FAILURE, sockfd, NULL);
    }

    printf("Starting thread\n");
    /* start a thread to refresh the client (handle egress and ingree client traffic) */
    pthread_t client_daemon;
    if(pthread_create(&client_daemon, NULL, clientRefresher, &client)) {
        printf("Failed to start client daemon.\n");
        exitExample(EXIT_FAILURE, sockfd, NULL);

    }

    /* start publishing the time */
    printf("PhiGatewy is ready to begin publishing the time.\n");
    
    /* get the current time */
    time_t timer;
    time(&timer);
    struct tm* tm_info = localtime(&timer);
    char timebuf[26];
    strftime(timebuf, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    /* print a message */
    char application_message[256];
    snprintf(application_message, sizeof(application_message), "The time is %s", timebuf);
    printf("PhiGateway published : \"%s\"", application_message);

    /* publish the time */
    mqtt_publish(&client, topic, application_message, strlen(application_message) + 1, MQTT_PUBLISH_QOS_0);

    printf("Message published\n");
    /* check for errors */
    if (client.error != MQTT_OK) {
        fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
        exitExample(EXIT_FAILURE, sockfd, &client_daemon);
    }


    /* disconnect */
    printf("\nPhiGateway disconnecting from %s\n", brokerAddr);
    sleep(1);

    /* exit */ 
    exitExample(EXIT_SUCCESS, sockfd, &client_daemon);

    return 0;
}

int PhiGateway::run(void) {

    printf("Iniciando aplicacao\n");
    simplePublisher();

    return 0;
}

void PhiGateway::exitExample(int status, int sockfd, pthread_t *client_daemon) {
    if (sockfd != -1) {
        close(sockfd);
    }
    if (client_daemon != NULL) {
        pthread_cancel(*client_daemon);
    }
    exit(status);
}

void PhiGateway::publishCallback(void** unused, struct mqtt_response_publish *published) {
    /* not used in this example */
}

void* PhiGateway::clientRefresher(void* client) {
    while(1)  {
        mqtt_sync((struct mqtt_client*) client);
        usleep(100000U);
    }
    return NULL;
}

