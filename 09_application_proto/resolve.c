/* Resolve DNS name into ip addresses */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    char *hostname = NULL, *servname = NULL;
    if (argc < 2) {
        printf("Usage: %s <hostname> [servname]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    hostname = argv[1];
    if (argc > 2) {
        servname = argv[2];
    }

    struct addrinfo *result, *rp, hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_V4MAPPED | AI_ALL;

    int rv = getaddrinfo(hostname, servname, &hints, &result);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }
    char addr[INET6_ADDRSTRLEN];

    struct sockaddr_in *addr_in = NULL;
    struct sockaddr_in6 *addr_in6 = NULL;
    void *addr_ptr = NULL;
    unsigned short port = 0;
    printf("Following addresses found for %s hostname:\n", hostname);
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        switch (rp->ai_family) {
            case AF_INET:
                addr_in = (struct sockaddr_in *) rp->ai_addr;
                addr_ptr = &(addr_in->sin_addr);
                port = ntohs(addr_in->sin_port);
                break;
            case AF_INET6:
                addr_in6 = (struct sockaddr_in6 *) rp->ai_addr;
                addr_ptr = &(addr_in6->sin6_addr);
                port = ntohs(addr_in6->sin6_port);
                break;
        }        
        if (inet_ntop(rp->ai_family, addr_ptr, addr, INET6_ADDRSTRLEN) == NULL) {
            perror("inet_ntop");
            exit(EXIT_FAILURE);
        }
        printf("    %s:%d\n", addr, port);
    }
    freeaddrinfo(result);
}
