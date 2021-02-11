#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char *argv[])
{
    unsigned char buf[sizeof(struct in6_addr)];
    int domain, s;
    char str[INET6_ADDRSTRLEN];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s {i4|i6} string\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "i4") == 0) {
        domain = AF_INET;
    } else if (strcmp(argv[1], "i6") == 0) {
        domain = AF_INET6;
    } else {
        fprintf(stderr, "Unsupported network protocol: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    s = inet_pton(domain, argv[2], buf);
    if (s <= 0) {
        if (s == 0)
            fprintf(stderr, "Not in presentation format");
        else
            perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    int addr_len;
    switch(domain) {
        case AF_INET:
            addr_len = sizeof(struct in_addr);
            break;
        case AF_INET6:
            addr_len = sizeof(struct in6_addr);
            break;
        default:
            fprintf(stderr, "Unsupported network protocol: %d\n", domain);
            exit(EXIT_FAILURE);
    }

    for (int i = 0; i < addr_len; ++i) {
        printf("%02x", buf[i]);
    }
    printf("\n");
    if (inet_ntop(domain, buf, str, INET6_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", str);

    exit(EXIT_SUCCESS);
}
