#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../headers/routing_map.h"

#define MAX_LOAD_FACTOR 0.7

int hash_into_idx(char * key, int key_len, int cap) {
    int sum = 0;
    for (int i = 0; i < key_len; i++) {
        sum += (int)key[i] * (i * i);
    }
    return sum % cap;
}

// void print_map(Map * m) {
//     for (int i = 0; i < m->bucket_count; i++) {
//         printf("Bucket %d [", i);
//         for (int j = 0; j < m->buckets[i]->len; j++) {
//             printf("{k: %s, v: %d, l: %d}, ", m->buckets[i]->pairs[j].key, m->buckets[i]->pairs[j].value, m->buckets[i]->pairs[j].key_len);
//         }
//         printf("], len: %d\n", m->buckets[i]->len);
//     }
// }

void handle_uri(RoutingMap * m, char * uri, void(*handler), int uri_len) {
    int b_idx = hash_into_idx(uri, uri_len, m->bucket_count);
    Bucket * bucket = m->buckets[b_idx];
    
    if (bucket->len == bucket->cap) {
        bucket->cap *= 1.5;
        bucket->pairs = realloc(bucket->pairs, bucket->cap * sizeof(KeyPair));
    }
    
    bucket->pairs[bucket->len].endpoint_uri = uri;
    bucket->pairs[bucket->len].endpoint_handler = handler;
    bucket->pairs[bucket->len].key_len = uri_len;
    bucket->len += 1;
    m->item_count += 1;

    if ((float)m->item_count / (float)m->bucket_count > MAX_LOAD_FACTOR) {
        m->bucket_count *= 2;
        Bucket ** new_buckets = malloc( m->bucket_count * sizeof(Bucket*));
        for (int i = 0; i < m->bucket_count; i++) {
            Bucket * b = malloc(sizeof(Bucket));
            b->len = 0;
            b->cap = 3;
            b->pairs = malloc(sizeof(KeyPair) * b->cap);
            new_buckets[i] = b;
        }

        for (int i = 0; i < (m->bucket_count)/2; i++) {
            for (int j = 0; j < m->buckets[i]->len; j++) {
                KeyPair kp = m->buckets[i]->pairs[j];
                int new_idx = hash_into_idx(kp.endpoint_uri, kp.key_len, m->bucket_count);
                Bucket * new_b = new_buckets[new_idx];

                if (new_b->len == new_b->cap) {
                    new_b->cap *= 1.5;
                    new_b->pairs = realloc(new_b->pairs, new_b->cap * sizeof(KeyPair));
                }

                new_b->pairs[new_b->len].endpoint_uri = kp.endpoint_uri;
                new_b->pairs[new_b->len].endpoint_handler = kp.endpoint_handler;
                new_b->pairs[new_b->len].key_len = kp.key_len;
                new_b->len += 1;
            }
        }
        for(int i = 0; i < m->bucket_count/2; i++){
            free(m->buckets[i]->pairs);
            free(m->buckets[i]);
        }
        free(m->buckets);
        m->buckets = new_buckets;
    }
}

KeyPair * get_handler(RoutingMap * m, char * uri, int uri_len){
    int b_idx = hash_into_idx(uri, uri_len, m->bucket_count);
    Bucket * bucket = m->buckets[b_idx];

    if (bucket->len == 0) {
        return NULL;
    }

    if (bucket->len == 1) {
        return &(bucket->pairs[0]);
    }

    for (int i = 0; i < bucket->len; i++){
        if (strcmp(uri, bucket->pairs[i].endpoint_uri) == 0) {
            return &(bucket->pairs[i]);
        }
    }
    return NULL;
}


RoutingMap * new_map(){
    RoutingMap * m = malloc(sizeof(RoutingMap));
    m->item_count = 0;
    m->bucket_count = 10;
    m->buckets = (Bucket**)malloc(sizeof(Bucket*) * m->bucket_count);
    
    for (int i = 0; i < m->bucket_count; i++) {
        Bucket * b = malloc(sizeof(Bucket));
        b->len = 0;
        b->cap = 3;
        b->pairs = malloc(sizeof(KeyPair) * b->cap);
        m->buckets[i] = b;
    }
    
    return m;
}

void free_map(RoutingMap * m) {
    for (int i = 0; i < m->bucket_count; i++) {
        free(m->buckets[i]->pairs);
        free(m->buckets[i]);
    }
    free(m->buckets);
    free(m);
}
