#include <sys/socket.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "banned.h"
#include "socketf.h"

volatile sig_atomic_t running = true;

int open_socket(const int);
void interrupt_handler(int);
void set_signal(struct sigaction*);
void work(const int);

int main(int argc, char* argv[argc + 1])
{
	/*
	 * Creates TCP socket and listens. Receives message from client then returns
     * acknowledgement.
     */
	if (argc < 2) {
		printf("Please pass port number to command line.\n");
		exit(EXIT_FAILURE);
	}
	int port_number = atoi(argv[1]);
	int socket_fd = open_socket(port_number);
	if (socket_fd < 0) {
		exit(EXIT_FAILURE);
	}

	struct sigaction action;
	set_signal(&action);
	while (running) {
		work(socket_fd);
	}
	close(socket_fd);
	return EXIT_SUCCESS;
}

int open_socket(const int port_number)
{
	/*
	 * Opens socket at port number. Returns negative number on error, else returns
	 * socket file descriptor. 
	 */
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		perror("Error opening socket");
	} else {
		int set = 1;
		setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));
		sockaddr_in server_address = { .sin_family = AF_INET,
									   .sin_addr.s_addr = INADDR_ANY,
									   .sin_port = htons(port_number) };
		int binding =
			bind(socket_fd, (sockaddr*)&server_address, sizeof(server_address));
		if (binding < 0) {
			perror("Error on binding");
			return binding;
		}
		int n = listen(socket_fd, 5);
		if (n < 0) {
			perror("Error on listen");
			return n;
		}
	}
	return socket_fd;
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

void work(const int socket_fd)
{
	/*
	 * Performs server work: accepts connection on socket and forks. Child process
	 * reads message from client then writes an acknowledgement to the socket.
	 */
	sockaddr_in client_address;
	socklen_t client_size = sizeof(client_address);
	int new_socket_fd = accept(socket_fd, (sockaddr*)&client_address, &client_size);
	if (new_socket_fd == -1) {
		if (errno == EINTR) {
			return;
		}
		perror("Unable to connect");
		exit(EXIT_FAILURE);
	}
	char* client_message;
	pid_t pid = fork();
	if (pid == -1) {
		perror("Error forking");
	} else if (pid == 0) {
		client_message = read_string_from_socket(new_socket_fd);
		printf("Received '%s' from client.\n", client_message);
		write_string_to_socket(new_socket_fd, "Hello from the C server.");
		free(client_message);
		close(new_socket_fd);
		// close(socket_fd);
		exit(EXIT_SUCCESS);
	} // else {
	// 	close(new_socket_fd);
	// }
}