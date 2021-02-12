#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define BUFF_SIZE 4096

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

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	if (connect(sock, (struct sockaddr*)&server_address,
	            sizeof(server_address)) == -1) {
		perror("connect");
		exit(EXIT_FAILURE);
	}


	const char* msg = argv[3];
	int msglen = strlen(msg);
	int wb;
	while (msglen) {
		wb = write(sock, msg, msglen);
		if (wb == -1) {
			perror("write");
			exit(EXIT_FAILURE);
		}
		msglen -= wb;
		msg += wb;
	}
	if (-1 == shutdown(sock, SHUT_WR)) {
		perror("shutdown");
		exit(EXIT_FAILURE);
	}

	int rb;
	char buffer[BUFF_SIZE];
	while ((rb = read(sock, buffer, BUFF_SIZE - 1)) > 0) {
		buffer[rb] = '\0';
		printf("%s", buffer);
	}
	if (rb == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	}
	close(sock);
	return 0;
}
