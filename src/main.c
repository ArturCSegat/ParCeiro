#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <signal.h>

#include "../headers/network_utils.h"
#include "../headers/request.h"

int accepting = 1;

void stop(int si) {
    accepting = 0;
}

int main(void) {
    signal(SIGINT, stop);

    int listener = get_listening_sock_or_die("3000");
    
    while (accepting) {
        struct sockaddr new_addr;
        socklen_t addr_len = sizeof new_addr;
        int new_con;

        if ((new_con = accept(listener, &new_addr, &addr_len)) == -1) {
            perror("failed to accept");
            continue;
        }
        
        char r_msg[500];
        
        int bytes = recv(new_con, r_msg, sizeof r_msg, 0);
        
        if (bytes < 0) {
            perror("error receiving");
            close(new_con);
            continue;
        } else if (bytes == 0) {
            printf("no bytes received\n");
            close(new_con);
            continue;
        }

        printf("received %d bytes from socket %d msg is: \n", bytes, new_con);
        printf("%s\n", r_msg);

        Request * r = malloc(sizeof(Request));
        r->method = parse_verb(r_msg);
        r->headers = parse_headers(r_msg);
        r->content_string = parse_content_string(r, r_msg);

        print_request(r);
        free_request(r);
    }
}
