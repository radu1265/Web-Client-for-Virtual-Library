#ifndef _REQUESTS_
#define _REQUESTS_

#include <string>

using namespace std;	

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *id, char* token,
							char *cookies);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char* content_type, string body_data, char *token,
							char *cookies);

// computes and returns a DELETE request string (cookies can be NULL if not needed)
char *compute_delete_request(char *host, char *url, char *id, char* token,
							char *cookies);

#endif
