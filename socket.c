/* socket.c: Simple Socket Functions */

#include "spidey.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * Allocate socket, bind it, and listen to specified port.
 **/
int
socket_listen(const char *port)
{
    /* Lookup server address information */

    /* For each server entry, allocate socket and try to connect */
    return -1;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
