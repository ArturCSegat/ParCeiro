#include <stdlib.h>
#include <stdio.h>
#include "../../headers/request.h"
#include <string.h>

// example request:

// POST /new-user HTTP/1.1\r\n
// Host: localhost:3000\r\n                                      
// User-Agent: curl/7.81.0\r\n
// Accept: */*\r\n                                      
// Content-Type: application/json\r\n
// Content-Length: 44\r\n
// \r\n
// {"name":"your name","phonenumber":"111-111"}\r\n


// extracts the verb from an http request in string form
char * parse_verb(const char * http_r_string) {
    // delete = 6 char's + 1 for the terminator(0) = 7
    char * verb = malloc(7);
    int idx = 0;
    while(http_r_string[idx] != ' ' && idx < 6){
        verb[idx] = http_r_string[idx];
        idx += 1;
    }
    verb[idx] = 0;

    return verb;
}

HeaderList * parse_headers(const char * http_string) {
    int idx = 0;
    while(http_string[idx] != '\n') idx += 1;
    
    HeaderList * headers = malloc(sizeof(HeaderList));
    headers->arr = malloc(sizeof(struct header) * 5);
    headers->len = 0;
    headers->cap = 5;

    while(http_string[idx] + http_string[idx+1] + http_string[idx+2] != '\n' + '\r' * 2) {
        for (; http_string[idx] == '\n' || http_string[idx]=='\r'; idx ++);
        char * h_name = malloc(40);
        int n_cap = 40;
        int n_count = 0;
        for (; http_string[idx] != ':'; idx++) {
            if (n_count == n_cap) {
                n_cap *= 2;
                h_name = realloc(h_name, n_cap);
            }
            h_name[n_count] = http_string[idx];
            n_count += 1;
        }
        h_name[n_count] = 0;

        char * h_val = malloc(40);
        int v_cap = 40;
        int v_count = 0;
        for (idx += 2; http_string[idx] != '\r'; idx++) {
            if (v_count == v_cap) {
                v_cap *= 2;
                h_val = realloc(h_val, v_cap);
            }
            h_val[v_count] = http_string[idx];
            v_count += 1;
        }
        h_val[v_count] = 0;

        if (headers->len == headers->cap) {
            headers->cap += 5;
            headers->arr = realloc(headers->arr, headers->cap * sizeof(struct header));
        }
            
        headers->arr[headers->len].name = h_name;
        headers->arr[headers->len].value = h_val;
        headers->len += 1;
        
        
    }

    return headers;
}

int get_header_idx(Request * r, const char * name) {
    for (int i = 0; i < r->headers->len; i++) {
        if (strcmp(r->headers->arr[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

char * parse_content_string(Request * r, const char * http_string){
    int idx = 0;
    for(;http_string[idx] + http_string[idx+1] + http_string[idx+2] != '\n' + '\r' * 2; idx++);
    for (; http_string[idx] == '\n' || http_string[idx]=='\r'; idx ++);
    
    int c_len_i = get_header_idx(r, "Content-Length");
    if (c_len_i == -1) return "\0";
    
    int size = atoi(r->headers->arr[c_len_i].value);

    char * content_buffer = malloc(size);

    int i = 0;
    for (; i < size; i++) {
        content_buffer[i] = http_string[idx];
        idx += 1;
    }
    content_buffer[size+1] = 0;
    
    return content_buffer;
}

void print_request(Request * r) {
    printf("Method: %s\n", r->method);
    printf("Headers: { ");
    for (int i = 0; i < r->headers->len; i++) {
        printf("%s: %s, ", r->headers->arr[i].name, r->headers->arr[i].value);
    }
    printf("}\n");
    printf("Content: %s\n", r->content_string);
}

Request * build_request(const char * http_string);

void free_request(Request * r) {
    free((char*)r->method);
    for (int i = 0; i < r->headers->len; i++) {
        free((char*)r->headers->arr[i].name);
        free((char*)r->headers->arr[i].value);
    }
    free(r->headers->arr);
    free(r->headers);
    if (r->content_string[0] != '\0') {
        free(r->content_string);
    }
    free(r);
}
