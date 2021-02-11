/* UDP echo server */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
	if (argc < 4) {
		printf("Usage: %s <hostname> <port> msg\n", argv[0]);
		exit(EXIT_FAILURE);
	}
    char* hostname = argv[1];
	int port = atoi(argv[2]);

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
    
	if (-1 == inet_pton(AF_INET, hostname, &server_address.sin_addr)) {
		perror("inet_pton");
		exit(EXIT_FAILURE);
	}
    server_address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
    const char *msg = argv[3];
    
	ssize_t len =
	    sendto(sock, msg, strlen(msg), 0,
	           (struct sockaddr*)&server_address, sizeof(server_address));
	if (len == -1) {
		perror("sendto");
		exit(EXIT_FAILURE);
	}
	char buffer[BUF_SIZE];
	ssize_t rb = recvfrom(sock, buffer, BUF_SIZE - 1, 0, NULL, NULL);
	if (rb == -1) {
		perror("recvfrom");
		exit(EXIT_FAILURE);
	}
	buffer[rb] = '\0';
	printf("recieved: '%s'\n", buffer);

	// close the socket
	close(sock);

    return EXIT_SUCCESS;
}
