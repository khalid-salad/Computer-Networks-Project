#ifndef SOCKETF_H
#define SOCKETF_H

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
typedef struct hostent hostent;

size_t read_length_from_socket(const int);
char* read_string_from_socket(const int);
int write_length_to_socket(const int, const size_t);
ssize_t write_string_to_socket(const int, const char* const);
void print_request(const char* const);

#endif
