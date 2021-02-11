/* Simple HTTP get client */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

const size_t buff_size = 4096;

const char *request_template = 
"GET %s HTTP/1.0\r\n"
"\r\n";

const size_t request_size = 4096;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <hostname> <uri>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *hostname = argv[1];
    const char *uri = argv[2];

    struct addrinfo *result, *rp, hints;
    int sfd;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;

    int rv = getaddrinfo(hostname, "http", &hints, &result);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }
    int connected = 0;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
        if (sfd == -1) {
            perror("socket");
            continue;
        }
        struct sockaddr_in *addr = (struct sockaddr_in *)rp->ai_addr;
        printf("connectiong to %s:%d\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
        if (-1 == connect(sfd, rp->ai_addr, rp->ai_addrlen)) {
            perror("connect");
            continue;
        }
        connected = 1;
        break;
    }
    freeaddrinfo(result);

    if (!connected) {
        fprintf(stderr, "Connection failed\n");
        exit(EXIT_FAILURE);
    }

    char request[request_size];
    
    rv = snprintf(request, request_size, request_template, 
                    uri);
    if (rv == -1) {
        perror("snprintf");
        exit(EXIT_FAILURE);
    }
    request[rv] = '\0';
    printf("%s", request);
    if (-1 == write(sfd, request, rv)) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    int rb;
    char buffer[buff_size];
    while ((rb = read(sfd, buffer, buff_size - 1)) > 0) {
        buffer[rb] = '\0';
        printf("%s", buffer);
    }
    if (rb == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(sfd);

    return EXIT_SUCCESS;
}
