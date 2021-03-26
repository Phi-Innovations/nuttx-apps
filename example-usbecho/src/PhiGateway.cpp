#include "PhiGateway.h"

#include <debug.h>
#include <errno.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/boardctl.h>
#include <nuttx/usb/usbdev.h>
#include <nuttx/usb/cdcacm.h>

PhiGateway::PhiGateway() { }

int PhiGateway::run(void) {

    printf("Starting application\n");
    startUSBEchoServer();

    return 0;
}

int PhiGateway::startUSBDevice(void) {
    struct boardioc_usbdev_ctrl_s ctrl;
    int ret;

    /* Check if there is a non-NULL USB mass storage device handle (meaning that the
    * USB mass storage device is already configured).
    */

    if (g_cdcacm.handle) {
        printf("sercon:: ERROR: Already connected\n");
        return -1;
    }

    /* Then, in any event, enable trace data collection as configured BEFORE
    * enabling the CDC/ACM device.
    */

    usbtrace_enable(TRACE_BITSET);

    /* Initialize the USB CDC/ACM serial driver */

    printf("USB Device: Registering CDC/ACM serial driver\n");

    ctrl.usbdev   = BOARDIOC_USBDEV_CDCACM;
    ctrl.action   = BOARDIOC_USBDEV_CONNECT;
    ctrl.instance = CONFIG_SYSTEM_CDCACM_DEVMINOR;
    ctrl.handle   = &g_cdcacm.handle;

    ret = boardctl(BOARDIOC_USBDEV_CONTROL, (uintptr_t)&ctrl);
    if (ret < 0) {
        printf("USB Device: ERROR: Failed to create the CDC/ACM serial device: %d\n", -ret);
        return -1;
    }

    printf("sercon: Successfully registered the CDC/ACM serial driver\n");

    return 0;
}

int PhiGateway::stopUSBDevice(void) {
    struct boardioc_usbdev_ctrl_s ctrl;
    
    /* First check if the USB mass storage device is already connected */
    if (!g_cdcacm.handle) {
        printf("USB Device: ERROR: Not connected\n");
        return -1;
    }

    /* Then, in any event, disable trace data collection as configured BEFORE
     * enabling the CDC/ACM device.
     */
    usbtrace_enable(0);

    /* Then disconnect the device and uninitialize the USB mass storage driver */
    ctrl.usbdev   = BOARDIOC_USBDEV_CDCACM;
    ctrl.action   = BOARDIOC_USBDEV_DISCONNECT;
    ctrl.instance = CONFIG_SYSTEM_CDCACM_DEVMINOR;
    ctrl.handle   = &g_cdcacm.handle;

    boardctl(BOARDIOC_USBDEV_CONTROL, (uintptr_t)&ctrl);
    g_cdcacm.handle = NULL;
    printf("USB Device: Disconnected\n");
    return 0;
}

int PhiGateway::startUSBEchoServer(void) {
    int fd;
    char buf = 0;
    bool fileOpen = false;

    printf("Inicializando a porta USB\n");
    startUSBDevice();

    printf("Abrindo a serial\n");
    while (fileOpen == false) {
        fd = open("/dev/ttyACM0", O_RDWR);
        if (fd < 0) {
            printf("ERROR: open failed: %d\n", errno);
            usleep(10000);
        }
        else {
            printf("Arquivo USB aberto\n");
            fileOpen = true;
        }
    }

    printf("Iniciando o echo server\n");
    while (1) {
        fflush(stdout);
        ssize_t nr = read(fd, &buf, 1);
        if (nr < 0) {
            printf("read failed: %d\n", errno);
            fflush(stdout);
            break;
        }
        else if (nr > 0) {
            ssize_t nw = write(fd, &buf, 1);
            if (nw < 0) {
                printf("write failed: %d\n", errno);
                fflush(stdout);
            }
            else if (nw != nr) {
                printf("problema no envio do retorno: %d\n",nw);
            }
        }
    }

    printf("Encerrando serial USB\n");
    close(fd);

    printf("Finalizando a porta USB\n");
    stopUSBDevice();

    return 0;
}

