/* spidey: Simple HTTP Server */

#include "spidey.h"

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

/* Global Variables */
char *Port	      = "9898";
char *MimeTypesPath   = "/etc/mime.types";
char *DefaultMimeType = "text/plain";
char *RootPath	      = "www";
mode  ConcurrencyMode = SINGLE;

/**
 * Display usage message.
 */
void
usage(const char *progname, int status)
{
    fprintf(stderr, "Usage: %s [hcmMpr]\n", progname);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -h            Display help message\n");
    fprintf(stderr, "    -c mode       Single or Forking mode\n");
    fprintf(stderr, "    -m path       Path to mimetypes file\n");
    fprintf(stderr, "    -M mimetype   Default mimetype\n");
    fprintf(stderr, "    -p port       Port to listen on\n");
    fprintf(stderr, "    -r path       Root directory\n");
    exit(status);
}

/**
 * Parses command line options and starts appropriate server
 **/
int
main(int argc, char *argv[])
{
    int sfd;
    char * temp;

    /* Parse command line options */
    char * PROGRAM_NAME = argv[0];
    if(argc == 1) usage(PROGRAM_NAME, 1);
    int argind = 1;
    while(argind < argc && strlen(argv[argind]) > 1 && argv[argind][0] == '-'){
        char * arg = argv[argind++];
        switch (arg[1]){
            case 'h':
                usage(PROGRAM_NAME, 0);
                break;
            case 'c':
                temp = argv[argind++];
                if(streq(temp, "forking"))
                    ConcurrencyMode = FORKING;
                else if(streq(temp, "single"))
                    ConcurrencyMode = SINGLE;
                else
                    ConcurrencyMode = UNKNOWN;
                break;
            case 'm':
                MimeTypesPath = argv[argind++];
                break;
            case 'M':
                DefaultMimeType = argv[argind++];
                break;
            case 'p':
                Port = argv[argind++];
                break;
            case 'r':
                RootPath = argv[argind++];
                break;
            default:
                usage(PROGRAM_NAME, 1);
                break;
        }
    }
                

    /* Listen to server socket */

    if((sfd = socket_listen(Port)) < 0){
        printf("Socket listen failed\n");
        return EXIT_FAILURE;
    }

    /* Determine real RootPath */
    RootPath = realpath(RootPath, NULL);

    log("Listening on port %s", Port);
    debug("RootPath        = %s", RootPath);
    debug("MimeTypesPath   = %s", MimeTypesPath);
    debug("DefaultMimeType = %s", DefaultMimeType);
    debug("ConcurrencyMode = %s", ConcurrencyMode == SINGLE ? "Single" : "Forking");

    /* Start either forking or single HTTP server */

    switch(ConcurrencyMode){
        case SINGLE:
            single_server(sfd);
            break;
        case FORKING:
            forking_server(sfd);
            break;
        case UNKNOWN:
            printf("Unknown Concurrency Mode");
            break;
    }


    return EXIT_SUCCESS;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
