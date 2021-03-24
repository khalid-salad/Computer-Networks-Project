#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include "banned.h"
#include "socketf.h"

int create_socket(const char* const, const int);
void print_message(const char* const, const char* const);

int main(int argc, char* argv[argc + 1])
{
	/*
	 * Client prompts user for host name and port number, then attempts to connect
	 * to server. If successful, it prompts the user for an email, then sends it to
	 * the server. It receives the public key (NULL if none exists) from the server
	 * then prints it.
	 */
	if (argc < 4) {
		printf("Please pass hostname, port number, and message.");
	}
	char* host_name = argv[1];
	int port_number = atoi(argv[2]);
	char* message = argv[3];
	const int socket_fd = create_socket(host_name, port_number);
	if (socket_fd < 0) { // socket failed
		exit(EXIT_FAILURE);
	}
	write_string_to_socket(socket_fd, message);
	char* acknowledgement = read_string_from_socket(socket_fd);
	if (errno == 0) { // key successfully read from server
		printf("%s\n", acknowledgement);
	}
	free(acknowledgement);
	return EXIT_SUCCESS;
}

int create_socket(const char* const host_name, const int port_number)
{
	/*
	 * Creates client socket given host_name and port_number.
	 */
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		perror("Error opening socket");
	} else {
		sockaddr_in server_address = { .sin_family = AF_INET,
									   .sin_port = htons(port_number) };
		hostent* server = gethostbyname(host_name);
		if (server == NULL) {
			fprintf(stderr, "Error, no such host\n");
			exit(EXIT_FAILURE);
		}
		memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
		int n = connect(socket_fd, (sockaddr*)&server_address, sizeof(server_address));
		if (n < 0) {
			perror("Error connecting");
			return n;
		}
	}
	return socket_fd;
}