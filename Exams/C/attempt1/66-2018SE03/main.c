#include <err.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
void checkNums(char* arg, int* start, int* end) {
	int len = strlen(arg);
	if(len != 1 && len != 3)
		errx(1, "ERROR: invalid second argument len");
    if(len == 3) {
		char first = arg[0];
		char second = arg[1];
        char third = arg[2];
        if((first < '0' || first > '9') || second != '-' || (third < '0' || third > '9'))
			errx(1, "Invalid second argument");
		*start = first - '0';
		*end = third - '0';
    } else {
		char first = arg[0];
		if(first < '0' || first > '9')
			errx(1, "Invalid second argument");
		*start = *end = first - '0';
	}
}

int main(int argc, char** argv) {
	bool C = false;
	bool D = false;
	int start = -1;
	int end = -1;
	char delim = '\0';

	if(argc != 3 && argc != 5)
		errx(1, "ERROR: params count");

	if(strcmp("-c", argv[1]) == 0) {
		C = true;
		checkNums(argv[2], &start, &end);
	} else if(strcmp("-d", argv[1]) == 0) {
		D = true;
		if(strcmp("-f", argv[3]))
			errx(1, "Missing -f argument");
		delim = argv[2][0];
		checkNums(argv[4], &start, &end);
	} else 
		errx(1, "Invalid Options");

	int bytes;
	char buf;
	int i = 0;
	--start;
	--end;
	if(C) {
		while((bytes = read(0, &buf, sizeof(buf))) > 0 && buf != '\n') {
			if(i >= start && i <= end)
				if(write(1, &buf, sizeof(buf)) == -1)
					err(3, "ERROR: writing to stdout with -c");
			++i;
		}
		if(bytes == -1)
			err(2, "ERROR: reading from stdin with -c");
	} else if (D) {
		while((bytes = read(0, &buf, sizeof(buf))) > 0 && buf != '\n') {
			if(buf == delim)
				++i;
			if((start == end && i == start) || (start != end && i >= start && i <= end))
				if(write(1, &buf, sizeof(buf)) == -1)
					err(3, "ERROR: writing to stdout with -d");
		}
		if(bytes == -1)
			err(2, "ERROR: reading from stdin with -d");
	}

	printf("\n");

	return 0;
}
