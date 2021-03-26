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

#include "libmodbus/modbus.h"

int PhiGateway::run(void) {

    printf("Starting application\n");
    startModbusMaster();

    return 0;
}

int PhiGateway::startModbusMaster(void) {
    printf("Starting modbus master\n");

    modbus_t *ctx = modbus_new_rtu("/dev/ttyS2",9600,'N',8,2);
    if (ctx == NULL) {
        printf("Error initializing modbus structure\n");
        return -1;
    }

    modbus_set_response_timeout(ctx,5,0);
	modbus_set_indication_timeout(ctx,3,0);

    modbus_set_debug(ctx,1);

    if (modbus_connect(ctx) == -1) {
        printf("Problem connecting modbus\n");
        modbus_free(ctx);
        return -1;
    }

    printf("Starting reading registers\n");
    while(1) {
        uint16_t table[2];

        memset((uint8_t*)table,0,sizeof(table));
        
        printf("Setting slave\n");
        if (modbus_set_slave(ctx, 1) == -1) {
            printf("Problem setting modbus slave to read\n");
            break;
        }

        printf("Reading registers\n");
        int rc = modbus_read_registers(ctx,10,2,table);
        if (rc < 0) {
            printf("Problem reading register: %d (%s)\n",rc, modbus_strerror(errno));
        }
        else {
            printf("Registers: %d %d\n",table[0], table[1]);
        }

        sleep(10);
    }

    printf("Disconnecting\n");
    modbus_close(ctx);

    printf("Freeing modbus\n");
    modbus_free(ctx);

    return -1;
}

