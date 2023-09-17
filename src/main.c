#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>

#include "../headers/network_utils.h"
#include "../headers/request.h"


int main(void) {
    int listener = get_listening_sock_or_die("3000");
    
    bool accepting = true;
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

        char * verb = parse_verb(r_msg);
        
        printf("the http verb of the request is : %s\n", verb);
        free(verb);

        HeaderList * headers = parse_headers(r_msg);

        print_headers(headers);

        Request * r = malloc(sizeof(Request));
        r->headers = headers;

        free_request(r);
    }
}
