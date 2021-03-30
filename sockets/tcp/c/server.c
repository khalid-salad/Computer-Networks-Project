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

volatile sig_atomic_t running = true;

void handle_client(int socket_fd)
{
	char buffer[128];
	int bytes_received = recv(socket_fd, buffer, 128, 0);
	if (bytes_received < 0) {
		perror("Recv failed:");
		exit(0);
	}
	printf("Received message from client: %s\n", buffer);
	char response[] = "Hello from the C server";
	send(socket_fd, response, strlen(response), 0);
}

void interrupt_handler(int signum)
{
	/*
	 * Sets global variable to intercept SIGINT.
	 */
	if (signum == SIGINT) {
		running = false;
	}
}

void set_signal(struct sigaction* action_ptr)
{
	/*
	 * Sets sigaction to intercept SIGINT. Sets sa_flags to disable
	 * SA_RESTART so that server does not hang while waiting on accept.
	 */
	sigemptyset(&action_ptr->sa_mask);
	action_ptr->sa_handler = interrupt_handler;
	action_ptr->sa_flags = SA_NOCLDSTOP | SA_ONSTACK;
	if (sigaction(SIGINT, action_ptr, NULL) == -1) {
		perror("sigaction");
	}
}

int main(int argc, char* argv[argc + 1])
{
	if (argc < 2) {
		printf("Please pass port number to command line.\n");
		exit(EXIT_FAILURE);
	}
	int port_number = atoi(argv[1]);
	int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		perror("Error creating socket:");
		exit(EXIT_FAILURE);
	}
	int set = 1;
	int ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));
	if (ret < 0) {
		perror("setsockopt failed");
		return -1;
	}
	struct sockaddr_in6 server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin6_family = AF_INET6;
	server_address.sin6_port = htons(port_number);
	server_address.sin6_addr = in6addr_any;
	ret = bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address));
	if (ret < 0) {
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}
	ret = listen(socket_fd, 1);
	if (ret < 0) {
		perror("Listen failed");
		exit(EXIT_FAILURE);
	}
	if (ret < 0) {
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
	struct sigaction action;
	set_signal(&action);
	while (running) {
		int new_socket_fd = accept(socket_fd, NULL, NULL);
		if (new_socket_fd < 0) {
			perror("Error accepting");
			break;
		}
		handle_client(new_socket_fd);
		close(new_socket_fd);
	}
	close(socket_fd);
	return EXIT_SUCCESS;
}