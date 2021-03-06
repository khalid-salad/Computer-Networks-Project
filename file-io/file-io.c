#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int write(char const* const);
int read(char const* const);

const char* FILENAME = "/tmp/c_test.txt";

int main(void)
{
	write(FILENAME);
	read(FILENAME);
	return EXIT_SUCCESS;
}

int write(char const* const filename)
{
	FILE* fp = fopen(filename, "w");
	if (!fp) {
		fprintf(stderr, "Error opening %s, %s.\n", filename, strerror(errno));
		return EXIT_FAILURE;
	}
	fputs("This is a C test file.\n", fp);
	for (int i = 1; i <= 10; i++) {
		fprintf(fp, "This is C test line %i.\n", i);
	}
	if (fclose(fp) == EXIT_SUCCESS) {
		return EXIT_SUCCESS;
	} else {
		fprintf(stderr, "Error closing %s, %s.\n", filename, strerror(errno));
		return EXIT_FAILURE;
	};
}

int read(char const* const filename)
{
	FILE* fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, "Error opening %s, %s.\n", filename, strerror(errno));
		return EXIT_FAILURE;
	}
	char* buffer = NULL;
	size_t buf_size = 0;
	ssize_t line_size;
	while ((line_size = getline(&buffer, &buf_size, fp)) >= 0) {
		printf("%s", buffer);
	}
	free(buffer);
	if (fclose(fp) != EXIT_SUCCESS) {
		fprintf(stderr, "Error closing %s, %s.\n", filename, strerror(errno));
		return EXIT_FAILURE;
	};
	if (line_size == -1 && errno != EXIT_SUCCESS) {
		fprintf(stderr, "Error reading %s, %s.\n", filename, strerror(errno));
		return EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}
}