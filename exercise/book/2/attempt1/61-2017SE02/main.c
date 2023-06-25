#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <fcntl.h>

void myRead(int fd, bool numbers, int* line);
void myRead(int fd, bool numbers, int* line) {
	bool toPrint = true;
	char buf;
	int readBytes;
	while((readBytes = read(fd, &buf, sizeof(buf)) > 0)) {
		if(numbers && toPrint) {
			printf("%d ", *line);
			(*line)++;
			toPrint = false;
		}

		if(write(1, &buf, sizeof(buf)) == -1)
			err(1, "ERROR: writing to stdout");

		if(numbers && buf == '\n')
			toPrint = true;
	}
	if(readBytes != 0)
		err(1, "ERROR: reading from stdin");
}

int main(int argc, char ** argv) {
	setbuf(stdout, NULL);

	bool numbers = false;
	bool isStdin = false;
	
	if(argc == 1) {
		isStdin = true;
	} else if (argc == 2 && strcmp(argv[1], "-n") == 0) {
		numbers = isStdin = true;
	} else if (strcmp(argv[1], "-n") == 0) {
		numbers = true;
	}
	

	int line = 1;

	if (isStdin) {
		myRead(0, numbers, &line);
	} else {
		for(int i = 1 + numbers; i < argc; ++i) {
			int fd;
			if(strcmp("-", argv[i]) == 0)
				fd = 0;
			else 
				if((fd = open(argv[i], O_RDONLY)) == -1)
					err(2, "ERROR: opening file for read: %s", argv[i]);
			
			myRead(fd, numbers, &line);

			close(fd);
		}	
	}

	exit(0);
}
