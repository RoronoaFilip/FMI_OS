#include <unistd.h>
#include <string.h>
#include <err.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
int charIndex(char c, char* str) {
	for(int i = 0; str[i] != '\0'; ++i)
		if(str[i] == c)
			return i;

	return -1;
}

int main(int argc, char** argv) {
	bool S = false;
	bool D = false;

	if(argc != 3)
		errx(1, "ERROR: params count");

	if(strcmp("-s", argv[1]) == 0) {
		S = true;
	} else if(strcmp("-d", argv[1]) == 0) {
		D = true;
	} else if(strlen(argv[1]) != strlen(argv[2])) {
		errx(1, "ERROR: first and second arguments are different lengths");
	}

	int bytes;
	char buf;
	char prev = '\0';

	while((bytes = read(0, &buf, sizeof(buf))) > 0) {
		if(S) {
			int index = charIndex(buf, argv[2]);
			if(prev == '\0' || index == -1 || (index != -1 && buf != prev)){
				if(write(1, &buf, sizeof(buf)) == -1)
					err(3, "ERROR: writing to stdout with -s");
			}

			prev = buf;
		} else if(D) {
			if(charIndex(buf, argv[2]) == -1)
				if(write(1, &buf, sizeof(buf)) == -1)
					err(3, "ERROR: writing to stdout with -d");
		} else {
			int index = charIndex(buf, argv[1]);

			if(index != -1)
				buf = argv[2][index];

			if(write(1, &buf, sizeof(buf)) == -1)
				err(3, "ERROR: writing to stdout with no options");
		}	
	}
	if(bytes != 0)
		err(2, "ERROR: reading from stdin");

	return 0;
}
