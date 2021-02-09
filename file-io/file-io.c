#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int write(const char*);
int read(const char*);

int main(void)
{
	write("/tmp/c_test.txt");
	read("/tmp/c_test.txt");
	return EXIT_SUCCESS;
}

int write(const char* filename)
{
	FILE* fp = fopen(filename, "w");
	if (!fp) {
		fprintf(stderr, "Error opening %s.\n", filename);
		return EXIT_FAILURE;
	}
	fputs("This is a test file.\n", fp);
	for (int i = 1; i <= 10; i++) {
		fprintf(fp, "This is line %i.\n", i);
	}
	if (fclose(fp) == 0) {
		return EXIT_SUCCESS;
	} else {
		fprintf(stderr, "Error closing %s, %s.\n", filename, strerror(errno));
		return EXIT_FAILURE;
	};
}

int read(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, "Error opening %s, error %s.\n", filename, strerror(errno));
		return EXIT_FAILURE;
	}
	char* buffer = NULL;
	size_t buf_size = 0;
	ssize_t line_size;
	while ((line_size = getline(&buffer, &buf_size, fp)) > 0) {
		printf("%s", buffer);
	}
	free(buffer);
	if (fclose(fp) != 0) {
		fprintf(stderr, "Error closing %s, %s.\n", filename, strerror(errno));
		return EXIT_FAILURE;
	};
	if (line_size == -1 && errno != EXIT_SUCCESS) {
		fprintf(stderr, "Error reading %s, error %s.\n", filename, strerror(errno));
		return EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}
}