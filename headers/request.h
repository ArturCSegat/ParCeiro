#include "header.h"
#ifndef REQUEST_H 
#define REQUEST_H

typedef struct {
    const char * method; // Method: GET, POST, DELETE etc
    HeaderList * headers;    // Headers of the request
    char * content_string;    // Request content
}Request;

char * parse_verb(const char * http_string);
HeaderList * parse_headers(const char * http_string);
char * parse_content_string(const char * http_string, int cont_len);

Request * build_request(const char * http_string);

void free_request(Request * r);

void print_request(Request * r);

int get_header_idx(Request * r, const char * name);

int validate_http_string(const char * http_string);

#endif
