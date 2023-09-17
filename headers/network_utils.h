#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

void *get_in_addr(struct sockaddr *sa);

struct addrinfo * get_sock_info(const char * ip, const char * port);

int get_server_sock_or_die(const char * server_ip, const char * port); 
int get_listening_sock_or_die(const char * port); 

#endif
