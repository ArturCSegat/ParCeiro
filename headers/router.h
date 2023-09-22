#include "routing_map.h"
#ifndef ROUTER_H
#define ROUTER_H

typedef struct {
    int router_fd;
    const char * tpc_port;
    RoutingMap * endpoint_map;   
}Router;

void listen_on(Router * r);

Router * new_router(const char * port);
#endif
