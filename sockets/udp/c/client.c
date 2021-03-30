#include <sys/socket.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "banned.h"
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[argc + 1])
{
	if (argc < 4) {
		printf("Please pass hostname, port number, and message.");
	}
	char* host = argv[1];
	char* port_number = argv[2];
	char* message = argv[3];
	char buffer[128];
	struct in6_addr server_address;
	struct addrinfo hints, *res = NULL;
	memset(&hints, 0x00, sizeof(hints));
	hints.ai_flags = AI_NUMERICSERV;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	int ret = inet_pton(AF_INET, host, &server_address);
	if (ret == 1) { /* valid IPv4 text address? */
		hints.ai_family = AF_INET;
		hints.ai_flags |= AI_NUMERICHOST;
	} else {
		ret = inet_pton(AF_INET6, host, &server_address);
		if (ret == 1) { /* valid IPv6 text address? */
			hints.ai_family = AF_INET6;
			hints.ai_flags |= AI_NUMERICHOST;
		}
	}
	ret = getaddrinfo(host, port_number, &hints, &res);
	if (ret != 0) {
		printf("Host not found --> %s\n", gai_strerror(ret));
		if (ret == EAI_SYSTEM) {
			perror("getaddrinfo failed");
			exit(EXIT_FAILURE);
		}
	}
	int socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (socket_fd < 0) {
		perror("Error creating socket");
		exit(EXIT_FAILURE);
	}
	ret = connect(socket_fd, res->ai_addr, res->ai_addrlen);
	if (ret < 0) {
		perror("Connect failed");
		exit(EXIT_FAILURE);
	}
	ret = sendto(socket_fd, message, strlen(message), MSG_CONFIRM,
				 (const struct sockaddr*)&server_address, sizeof(server_address));
	if (ret < 0) {
		perror("Send failed");
		exit(EXIT_FAILURE);
	}
	unsigned int address_length;
	ret = recvfrom(socket_fd, buffer, 128, MSG_WAITALL,
				   (struct sockaddr*)&server_address, &address_length);
	if (ret < 0) {
		perror("Receive failed");
		exit(EXIT_FAILURE);
	} else if (ret == 0) {
		printf("The server closed the connection");
	} else {
		printf("Received message from server: %s\n", buffer);
	}
	close(socket_fd);
	if (res != NULL) {
		freeaddrinfo(res);
	}
	exit(EXIT_SUCCESS);
}
