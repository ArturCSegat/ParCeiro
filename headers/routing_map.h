#include "./request.h"
#ifndef ROUTING_MAP_H
#define ROUTING_MAP_H

typedef struct {
    char * endpoint_uri;
    void(*endpoint_handler)(Request*);
    int key_len;
}KeyPair;

typedef struct {
    KeyPair * pairs;
    int len;
    int cap;
}Bucket;

typedef struct {
    Bucket ** buckets;
    int item_count;
    int bucket_count;
}RoutingMap;

int hash_into_idx(char * key, int key_len, int cap);

void handle_uri(RoutingMap * m, char * uri, void(*handler), int uri_len);

KeyPair * get_handler(RoutingMap * m, char * uri, int uri_len);

RoutingMap * new_map();

void free_map(RoutingMap * m);
#endif
