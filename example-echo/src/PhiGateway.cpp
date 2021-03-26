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

#define TCPECHO_MAXLINE 64
#define TCPECHO_POLLTIMEOUT 10000


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

int PhiGateway::startEchoServer(void) {
    int i, maxi, listenfd, connfd, sockfd;
    int nready;
    int ret;
    ssize_t n;
    char buf[TCPECHO_MAXLINE];
    socklen_t clilen;
    bool stop = false;
    struct pollfd client[8];
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("ERROR: failed to create socket.\n");
        return ERROR;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(80);

    ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        perror("ERROR: failed to bind socket.\n");
        return ERROR;
    }

    ninfo("start listening on port: %d\n", 80);

    ret = listen(listenfd, 8);
    if (ret < 0) {
        perror("ERROR: failed to start listening\n");
        return ERROR;
    }

    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    for (i = 1; i < 8; i++) {
        client[i].fd = -1; /* -1 indicates available entry */
    }

    maxi = 0; /* max index into client[] array */

    while (!stop) {
        nready = poll(client, maxi + 1, TCPECHO_POLLTIMEOUT);

        if (client[0].revents & POLLRDNORM) {
            /* new client connection */

            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

            ninfo("new client: %s\n", inet_ntoa(cliaddr.sin_addr));

            for (i = 1; i < 8; i++) {
                if (client[i].fd < 0)
                {
                    client[i].fd = connfd; /* save descriptor */
                    break;
                }
            }

            if (i == 8) {
                perror("ERROR: too many clients");
                return ERROR;
            }

            client[i].events = POLLRDNORM;
            if (i > maxi) {
                maxi = i; /* max index in client[] array */
            }

            if (--nready <= 0) {
                continue; /* no more readable descriptors */
            }
        }

        for (i = 1; i <= maxi; i++) {
            /* check all clients for data */

            if ((sockfd = client[i].fd) < 0) {
                continue;
            }

            if (client[i].revents & (POLLRDNORM | POLLERR)) {
                if ((n = read(sockfd, buf, TCPECHO_MAXLINE)) < 0) {
                    if (errno == ECONNRESET) {
                        /* connection reset by client */

                        nwarn("WARNING: client[%d] aborted connection\n", i);

                        close(sockfd);
                        client[i].fd = -1;
                    }
                    else {
                        perror("ERROR: readline error\n");
                        close(sockfd);
                        client[i].fd = -1;
                    }
                }
                else if (n == 0) {
                    /* connection closed by client */

                    nwarn("WARNING: client[%d] closed connection\n", i);

                    close(sockfd);
                    client[i].fd = -1;
                }
                else {
                    if (strcmp(buf, "exit\r\n") == 0) {
                        nwarn("WARNING: client[%d] closed connection\n", i);
                        close(sockfd);
                        client[i].fd = -1;
                    }
                    else {
                        write(sockfd, buf, n);
                    }
                }

                if (--nready <= 0) {
                    break; /* no more readable descriptors */
                }
            }
        }
    }

    for (i = 0; i <= maxi; i++) {
        if (client[i].fd < 0) {
            continue;
        }

        close(client[i].fd);
    }

    return ret;
}

int PhiGateway::run(void) {

    printf("Starting Phi-Gateway\n");
    startEchoServer();

    return 0;
}


