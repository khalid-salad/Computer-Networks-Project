#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "banned.h"
#include "socketf.h"

size_t read_length_from_socket(const int socket_fd)
{
	/*
	 * Reads size_t value from socket and returns it.
	 */
	size_t buffer = 0;
	int n = recv(socket_fd, &buffer, sizeof(buffer), MSG_NOSIGNAL);
	if (n < 0) {
		perror("Error reading from socket");
		return 0;
	}
	return buffer;
}

char* read_string_from_socket(const int socket_fd)
{
	/*
	 * Reads string from socket and returns it.
	 */
	size_t length = read_length_from_socket(socket_fd);
	if (length == 0) {
		return NULL;
	}
	char* buffer = malloc(length + 1);
	if (buffer == NULL) {
		perror("Error allocating buffer");
		char* buffer = malloc(length + 1);
	}
	int n = recv(socket_fd, buffer, length + 1, MSG_NOSIGNAL);
	if (n < 0) {
		perror("Error reading from socket");
	}
	return buffer;
}

int write_length_to_socket(const int socket_fd, const size_t n)
{
	/*
	 * Write size_t value to socket. Return 0 on success, -1 on failure.
	 */
	int return_value = send(socket_fd, &n, sizeof(n), MSG_NOSIGNAL);
	if (return_value < 0) {
		perror("Error writing int to socket");
		return -1;
	}
	return 0;
}

ssize_t write_string_to_socket(const int socket_fd, const char* const string)
{
	/*
	 * Write string to socket. Returns string length (0 if string is NULL) 
	 * on success, -1 on failure. 
	 */
	size_t length;
	if (string == NULL) {
		length = 0;
	} else {
		length = strlen(string);
	}
	int integer_write = write_length_to_socket(socket_fd, length);
	if (integer_write < 0) {
		return -1;
	}
	if (length == 0) {
		return 0;
	}
	int return_value = send(socket_fd, string, length + 1, MSG_NOSIGNAL);
	if (return_value < 0) {
		perror("Error writing string to socket");
	}
	return return_value;
}