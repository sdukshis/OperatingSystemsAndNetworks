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
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t running = 0;
#define BUF_SIZE 4096

void signal_handler(int signal) {
	if (signal == SIGINT) {
    	running = 0;
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int server_port = atoi(argv[1]);

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(server_port);

    int sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	printf("Starting UDP server on %d\n", server_port);

    if ((bind(sock, (struct sockaddr *)&server_address,
	          sizeof(server_address))) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	struct sigaction action;
	action.sa_handler = signal_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
    if (-1 == sigaction(SIGINT, &action, NULL)) {
        perror("sigaction");
		exit(EXIT_FAILURE);
    }

	// socket address used to store client address
	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);
    char buffer[BUF_SIZE];

	printf("Waiting for incomming packets\n");
    for (running = 1; running;) {
        ssize_t len = recvfrom(sock, buffer, BUF_SIZE - 1, 0,
		                   (struct sockaddr *)&client_address,
		                   &client_address_len);
		if (len == -1 && errno) {
			if (errno == EINTR) {
				/* restart cycle body */
				continue;
			} 
			perror("recvfrom");
			exit(EXIT_FAILURE);
		}
        buffer[len] = '\0';
		printf("received: '%s' from client %s:%d\n", buffer,
		       inet_ntoa(client_address.sin_addr),
			   ntohs(client_address.sin_port));
		if (-1 == sendto(sock, buffer, len, 0, (struct sockaddr *)&client_address, client_address_len)) {
			perror("sendto");
			exit(EXIT_FAILURE);
		}
    }
	printf("Finishing server\n");

    close(sock);
    return 0;
}
