/* forking.c: Forking HTTP Server */

#include "spidey.h"

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <unistd.h>

/**
 * Fork incoming HTTP requests to handle the concurrently.
 *
 * The parent should accept a request and then fork off and let the child
 * handle the request.
 **/
void
forking_server(int sfd)
{
    struct request *request;
    pid_t pid;

	signal(SIGCHLD, SIG_IGN);
    /* Accept and handle HTTP request */
    while (true) {
    	/* Accept request */
        request = accept_request(sfd);
		if (request == NULL){
			continue;
		}

	/* Ignore children */

	/* Fork off child process to handle request */
        pid = fork();
		if (pid < 0){
			fprintf(stderr, "Unable to fork: %s\n", strerror(errno));
			fclose(request->file);
			continue;
		}
		else if (pid == 0){
    		/* Close server socket and exit*/
    		handle_request(request);
    		fclose(request->file);
    		exit(EXIT_SUCCESS);
		}else{
        	free_request(request);
		}
    }
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
