#include <cstdlib> // For exit, atoi, malloc, free
#include <cstdio> // For perror
#include <unistd.h> // For read, write, close
#include <cstring> // For memcpy, memset
#include <sys/socket.h> // For socket, connect
#include <netinet/in.h> // For struct sockaddr_in, struct sockaddr
#include <netdb.h> // For struct hostent, gethostbyname
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include <iostream>

using namespace std;

char* compute_get_request(char* host, char* url, char* id, char* token,
    char* cookies)
{
    char* message = (char*)calloc(BUFLEN, sizeof(char));
    char* line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (id != nullptr) {
        sprintf(line, "GET %s%s? HTTP/1.1", url, id);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (token != nullptr) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    if (cookies != nullptr) {
        // Add cookies here
        sprintf(line, "Cookie: connect.sid=%s", cookies);

        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}

char* compute_post_request(char* host, char* url, char* content_type, string body_data, char* token,
    char* cookies)
{
    char* message = (char*)calloc(BUFLEN, sizeof(char));
    char* line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
        in order to write Content-Length you must first compute the message size
    */
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", strlen(body_data.c_str()));
    compute_message(message, line);

    if (token != nullptr) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Step 4 (optional): add cookies

    if (cookies != nullptr) {
        // Add cookies here
        sprintf(line, "Cookie: ");
        compute_message(message, line);
    }
    // Step 5: add new line at end of header
    strcat(message, "\n");

    sprintf(line, "%s", body_data.c_str());
    compute_message(message, line);

    free(line);
    return message;
}

char* compute_delete_request(char* host, char* url, char* id, char* token,
    char* cookies)
{
    char* message = (char*)calloc(BUFLEN, sizeof(char));
    char* line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (id != nullptr) {
        sprintf(line, "DELETE %s%s? HTTP/1.1", url, id);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (token != nullptr) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    if (cookies != nullptr) {
        // Add cookies here
        sprintf(line, "Cookie: connect.sid=%s", cookies);

        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}