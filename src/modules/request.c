#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../../headers/request.h"

// extracts the verb from an http request in string form
char * parse_verb(char ** http_r_string) {
    char * verb = malloc(8);
    int i = 0;
    for(;**http_r_string != ' '; *http_r_string += 1){
        verb[i] = **http_r_string;
        i++;
    }
    verb[i+1] = 0;
    
    *http_r_string = strchr(*http_r_string, '\n');
    *http_r_string += 1;
    return verb;
}

HeaderList * parse_headers(char ** http_string) {
    HeaderList * headers = malloc(sizeof(HeaderList));
    headers->arr = malloc(sizeof(struct header) * 5);
    headers->len = 0;
    headers->cap = 5;

    for(;strncmp(*http_string, "\r\n\r\n", 4) != 0; *http_string += 1) {
        char * h_name = malloc(40);
        int n_cap = 40;
        int n_count = 0;
        char * h_val = malloc(40);
        int v_cap = 40;
        int v_count = 0;

        for (; **http_string != ':' && **http_string != '\r'; *http_string += 1) {
            if (n_count == n_cap) {
                n_cap *= 2;
                h_name = realloc(h_name, n_cap);
            }
            h_name[n_count] = tolower(**http_string);
            n_count += 1;
        }
        if (n_count == 0) {
            free(h_name);
            free(h_val);
            break;
        }

        h_name[n_count] = 0;

        for (*http_string += 2; **http_string != '\r'; *http_string += 1) {
            if (v_count == v_cap) {
                v_cap *= 2;
                h_val = realloc(h_val, v_cap);
            }
            h_val[v_count] = tolower(**http_string);
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

        if ((*http_string)[3] != '\r') {
            *http_string = strchr(*http_string, '\n');
        }
    }
    *http_string = strstr(*http_string, "\r\n") + 2; // skip \r\n\r\n at the end

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

char * parse_content_string(char ** http_string, int cont_len){

    if (cont_len <=0) {
        return "\0";
    }

    char * content_buffer = malloc(cont_len+1);

    int i = 0;
    for (; i < cont_len && **http_string != '\r'; i++) {
        content_buffer[i] = **http_string;
        *http_string += 1;
    }
    content_buffer[cont_len] = 0;
    if (i != cont_len) {
        free(content_buffer);
        return "\0";
    }
    
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

int validate_http_string(const char * http_string) {
    int len = strlen(http_string);
    // GET / HTTP/1.0\r\n
    if (len < 15) return -1;
    
    
    char * uri_start = strchr(http_string, ' ');

    if (uri_start == NULL) return -3;
    uri_start += 1;
    char * uri_end = strchr(uri_start, ' ');

    if (uri_end == NULL || uri_end == uri_start) return -4;
    char * version_start = strchr(uri_end, 'H');
    if (version_start == NULL || version_start == uri_end) return -5;
    char * version_end = strchr(version_start, '/');
    if (version_end == NULL || version_start == version_end) return -6;

    if (strncmp(version_start, "HTTP", 4) != 0) return -7;
    
    char * s_r = strchr(version_end, '\r');
    if (s_r == NULL) return -2;
    
    char * s_n = strchr(version_end, '\n');
    if (s_n == NULL) return -8;

    return 0;
}


Request * build_request(char * http_string) {
        Request * r = malloc(sizeof(Request));
        r->method = parse_verb(&http_string);
        printf("parsed verb: %s\n", r->method);
        printf("current: %s\n", http_string);
        r->headers = parse_headers(&http_string);
        
        int c_len = get_header_idx(r, "content-length");
        if (c_len == -1) {
            r->content_string = "\0";
        } else {
            r->content_string = parse_content_string(&http_string, atoi(r->headers->arr[c_len].value));
        }

        return r;
}
