/* handler.c: HTTP Request Handlers */

#include "spidey.h"

#include <errno.h>
#include <limits.h>
#include <string.h>

#include <dirent.h>
#include <unistd.h>

/* Internal Declarations */
http_status handle_browse_request(struct request *request);
http_status handle_file_request(struct request *request);
http_status handle_cgi_request(struct request *request);
http_status handle_error(struct request *request, http_status status);

/**
 * Handle HTTP Request
 *
 * This parses a request, determines the request path, determines the request
 * type, and then dispatches to the appropriate handler type.
 *
 * On error, handle_error should be used with an appropriate HTTP status code.
 **/
http_status
handle_request(struct request *r)
{
    http_status result;

    /* Parse request */
    if (parse_request(r) < 0){
        result = handle_error(r, HTTP_STATUS_BAD_REQUEST);
    }

    /* Determine request path */
    r->path = determine_request_path(r->uri);
    if (r->path == NULL){
        debug("Null request path");
        handle_error(r, HTTP_STATUS_BAD_REQUEST);
    }    
    debug("HTTP REQUEST PATH: %s", r->path);

    /* Dispatch to appropriate request handler type */
    request_type req = determine_request_type(r->path);
    if (req == REQUEST_BROWSE) result = handle_browse_request(r);
    else if (req == REQUEST_FILE) result = handle_file_request(r);
    else if (req == REQUEST_CGI) result = handle_cgi_request(r);
    else {
        debug("req");
        result = handle_error(r, HTTP_STATUS_NOT_FOUND);
    }
    log("HTTP REQUEST STATUS: %s", http_status_string(result));
    return result;
}

/**
 * Handle browse request
 *
 * This lists the contents of a directory in HTML.
 *
 * If the path cannot be opened or scanned as a directory, then handle error
 * with HTTP_STATUS_NOT_FOUND.
 **/
http_status
handle_browse_request(struct request *r)
{
    struct dirent **entries;
    int n;
    
    //debug("HBR Before: %s", r->path);
    /* Open a directory for reading or scanning */
    n = scandir(r->path, &entries, NULL, alphasort);
    //debug("HBR After: %s", r->path);
    if (n < 0){
        return handle_error(r, HTTP_STATUS_NOT_FOUND);
    }

    /* Write HTTP Header with OK Status and text/html Content-Type */
    fprintf(r->file, "HTTP/1.0 200 OK\r\n");
    fprintf(r->file, "Content-Type: text/html\r\n");
    fprintf(r->file, "\r\n");
    
    // Bootstrap Implementation
    fprintf(r->file, "<!DOCTYPE html>");
    fprintf(r->file, "<html lang=\"en\">");
    fprintf(r->file, "<head>");
    fprintf(r->file, "<title>Guru Point :)</title>");
    fprintf(r->file, "<meta name=\"viewport\" content=\"width=device-width, inital-scale=1\">");
    fprintf(r->file, "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">");
    fprintf(r->file, "</head>");
    fprintf(r->file, "<body>");
    
    /* For each entry in directory, emit HTML list item */
    fprintf(r->file, "<ul class=\"list-group\">");
    for (int i = 0; i < n; i++){
        if (streq(entries[i]->d_name, ".") || streq(entries[i]->d_name, "..")){
            continue;
        }
        // printing name of file in html
        fprintf(r->file, "<li class=\"list-group-item\"><a href = \"%s/%s\">%s</a></li>\n", streq(r->uri, "/") ? "" : r->uri, entries[i]->d_name, entries[i]->d_name);
    }
    
    fprintf(r->file, "</ul>"); // finish unordered list
    fprintf(r->file, "</body>");
    fprintf(r->file, "</html>");
    //free(entries); // deallocate
    
    /* Flush socket, return OK */
    fflush(r->file);
    return HTTP_STATUS_OK;
}

/**
 * Handle file request
 *
 * This opens and streams the contents of the specified file to the socket.
 *
 * If the path cannot be opened for reading, then handle error with
 * HTTP_STATUS_NOT_FOUND.
 **/
http_status
handle_file_request(struct request *r)
{
    FILE *fs;
    char buffer[BUFSIZ];
    char *mimetype = NULL;
    size_t nread;
    /* Open file for reading */
    if ((fs = fopen(r->path, "rb")) == NULL){
        fprintf(stderr, "fopen failed: %s\n", strerror(errno));
        fclose(fs);
        return handle_error(r, HTTP_STATUS_NOT_FOUND);
    }
    
    /* Determine mimetype */
    mimetype = determine_mimetype(r->path);

    /* Write HTTP Headers with OK status and determined Content-Type */
    fprintf(r->file, "HTTP/1.0 OK\n");
    fprintf(r->file, "Content-Type: %s\r\n", mimetype);
    fprintf(r->file, "\r\n");
    /* Read from file and write to socket in chunks */
    while ((nread = fread(buffer, sizeof(char), BUFSIZ, fs)) > 0){
        fwrite(buffer, sizeof(char), nread, r->file);
    }
    /* Close file, flush socket, deallocate mimetype, return OK */
    fclose(fs);
    fflush(r->file);
    free(mimetype);
    return HTTP_STATUS_OK;
}

/**
 * Handle file request
 *
 * This popens and streams the results of the specified executables to the
 * socket.
 *
 *
 * If the path cannot be popened, then handle error with
 * HTTP_STATUS_INTERNAL_SERVER_ERROR.
 **/
http_status
handle_cgi_request(struct request *r)
{
    FILE *pfs;
    char buffer[BUFSIZ];
    struct header *header;

    /* Export CGI environment variables from request:
    * http://en.wikipedia.org/wiki/Common_Gateway_Interface */
    if (setenv("DOCUMENT_ROOT", RootPath, 1) < 0){
        fprintf(stderr, "failed to set environmental variable: %s\n", strerror(errno));
        return HTTP_STATUS_INTERNAL_SERVER_ERROR;
    }
    if (setenv("QUERY_STRING", r->query, 1) < 0){
        fprintf(stderr, "failed to set environmental variable: %s\n", strerror(errno));
        return HTTP_STATUS_INTERNAL_SERVER_ERROR;

    }
    if (setenv("REMOTE_ADDR", r->host, 1) < 0){
        fprintf(stderr, "failed to set environmental variable: %s\n", strerror(errno));
        return HTTP_STATUS_INTERNAL_SERVER_ERROR;
   
    }
    if (setenv("REMOTE_PORT", r->port, 1) < 0){
        fprintf(stderr, "failed to set environmental variable: %s\n", strerror(errno));
        return HTTP_STATUS_INTERNAL_SERVER_ERROR;
   
    }
    if (setenv("REQUEST_METHOD", r->method, 1) < 0){
        fprintf(stderr, "failed to set environmental variable: %s\n", strerror(errno));
        return HTTP_STATUS_INTERNAL_SERVER_ERROR;
   
    }
    if (setenv("REQUEST_URI", r->uri, 1) < 0){
        fprintf(stderr, "failed to set environmental variable: %s\n", strerror(errno));
        return HTTP_STATUS_INTERNAL_SERVER_ERROR;
   
    }
    if (setenv("SERVER_PORT", r->port, 1) < 0){
        fprintf(stderr, "failed to set environmental variable: %s\n", strerror(errno));
        return HTTP_STATUS_INTERNAL_SERVER_ERROR;

    }
    if (setenv("SCRIPT_FILENAME", r->path, 1) < 0){
         fprintf(stderr, "failed to set environmental variable: %s\n", strerror(errno));
        return HTTP_STATUS_INTERNAL_SERVER_ERROR;
   
    }
       

    /* Export CGI environment variables from request headers */
    for (header = r->headers; header != NULL; header = header->next){
        if (streq(header->name, "Host")) 
            setenv("HTTP_HOST", header->value, 1);
        else if (streq(header->name, "Accept"))
            setenv("HTTP_ACCEPT", header->value, 1);
        else if (streq(header->name, "Accept-Language"))
            setenv("HTTP_ACCEPT_LANGUAGE", header->value, 1);
        else if (streq(header->name, "Accept-Encoding"))
            setenv("HTTP_ACCEPT_ENCODING", header->value, 1);
        else if (streq(header->name, "Connection"))
            setenv("HTTP_CONNECTION", header->value, 1);
        else if (streq(header->name, "User-Agent"))
            setenv("HTTP_USER_AGENT", header->value, 1);
    }
    /* POpen CGI Script */
    if ((pfs = popen(r->path, "r")) == NULL){
        pclose(pfs);
        return HTTP_STATUS_NOT_FOUND;
    }

    /* Copy data from popen to socket */
    while (fgets(buffer, BUFSIZ, pfs)){
        fputs(buffer, r->file);
    }

    /* Close popen, flush socket, return OK */
    pclose(pfs);
    fflush(r->file);
    return HTTP_STATUS_OK;
}

/**
 * Handle displaying error page
 *
 * This writes an HTTP status error code and then generates an HTML message to
 * notify the user of the error.
 **/
http_status
handle_error(struct request *r, http_status status)
{
    const char *status_string = http_status_string(status);

    /* Write HTTP Header */
    fprintf(r->file, "HTTP/1.0 %s\n", status_string);
    fprintf(r->file, "Content-Type: text/html\r\n");
    fprintf(r->file, "\r\n");

     // Bootstrap Implementation
    fprintf(r->file, "<!DOCTYPE html>");
    fprintf(r->file, "<html lang=\"en\">");
    fprintf(r->file, "<head>");
    fprintf(r->file, "<title>Guru Point :)</title>");
    fprintf(r->file, "<meta name=\"viewport\" content=\"width=device-width, inital-scale=1\">");
    fprintf(r->file, "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">");
    fprintf(r->file, "</head>");
    fprintf(r->file, "<body>");

    /* Write HTML Description of Error*/
    fprintf(r->file, "<p class=\"bg-primary\">%s</p>\n", status_string);
    fflush(r->file);

    /* Return specified status */
    return status;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
