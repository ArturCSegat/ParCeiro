#include "../../headers/router.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include "../../headers/network_utils.h"
#include "../../headers/routing_map.h"

void listen_on(Router *r, const char * port) {
    r->tpc_port = port;
    r->router_fd = get_listening_sock_or_die(port);
    r->endpoint_map = new_map();

    int accepting = 1;

    while (accepting) {
        printf("\n\n new \n\n");
        struct sockaddr new_addr;
        socklen_t addr_len = sizeof new_addr;
        int new_con;

        if ((new_con = accept(r->router_fd, &new_addr, &addr_len)) == -1) {
            perror("failed to accept");
            continue;
        }

        char r_msg[500];

        int bytes = recv(new_con, r_msg, sizeof r_msg, 0);

        if (bytes < 0) {
            perror("error receiving");
            close(new_con);
            memset(r_msg, 0, sizeof strlen(r_msg));
            continue;
        } else if (bytes == 0) {
            printf("no bytes received\n");
            close(new_con);
            memset(r_msg, 0, sizeof strlen(r_msg));
            continue;
        }
        r_msg[bytes] = '\0';       
        if (validate_http_string(r_msg) != 0) {
            printf("bad request happened\n");
            printf("err: %d\n", validate_http_string(r_msg));
            printf("\n\nfull raw message: %s", r_msg);
            printf("end of message\n");
            memset(r_msg, 0, sizeof strlen(r_msg));
            continue;
        }

        Request * r = build_request(r_msg);

        print_request(r);
        free_request(r);

        printf("\n\nfull raw message: %s\n", r_msg);

        memset(r_msg, 0, sizeof strlen(r_msg));
    }
}

Router * new_router(){
    return malloc(sizeof(Router));
}
