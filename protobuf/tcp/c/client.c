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
#include "person.pb-c.h"

int main(int argc, char* argv[argc + 1])
{
	if (argc < 3) {
		printf("Please pass hostname and port number.");
		exit(EXIT_FAILURE);
	}
	char* host = argv[1];
	char* port_number = argv[2];
	Tutorial__Person message = TUTORIAL__PERSON__INIT;
	char buffer[1024];
	const char msg[] = "C Client";
	unsigned int length = strlen(msg);
	message.name = malloc(length + 1);
	memcpy(message.name, msg, length + 1);
	message.name[length + 1] = 0;
	message.id = 1;
	length = tutorial__person__get_packed_size(&message);
	// unsigned int length = tutorial__person__get_packed_size(&message);
	struct in6_addr serveraddr;
	struct addrinfo hints, *res = NULL;
	memset(&hints, 0x00, sizeof(hints));
	hints.ai_flags = AI_NUMERICSERV;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	int ret = inet_pton(AF_INET, host, &serveraddr);
	if (ret == 1) { /* valid IPv4 text address? */
		hints.ai_family = AF_INET;
		hints.ai_flags |= AI_NUMERICHOST;
	} else {
		ret = inet_pton(AF_INET6, host, &serveraddr);
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
	if (socket_fd < 0) {
		exit(EXIT_FAILURE);
	}
	tutorial__person__pack(&message, (uint8_t*)buffer);
	ret = send(socket_fd, buffer, length, 0);
	if (ret < 0) {
		perror("Send failed");
		exit(EXIT_FAILURE);
	}
	ret = recv(socket_fd, buffer, 1024, 0);
	if (ret < 0) {
		perror("Receive failed");
		exit(EXIT_FAILURE);
	} else if (ret == 0) {
		printf("The server closed the connection");
	} else {
		Tutorial__Person* response =
			tutorial__person__unpack(NULL, ret, (uint8_t*)buffer);
		printf("Received message from server.\n");
		printf("name: %s\n", response->name);
		printf("id: %d\n", response->id);
	}
	close(socket_fd);
	if (res != NULL) {
		freeaddrinfo(res);
	}
	exit(EXIT_SUCCESS);
}
