#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <fcntl.h>

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
	
	bool toPrint = numbers;
	int i = 1;
	int el = 1 + numbers;
	int readBytes;
	char buf;

	if (isStdin) {
		while((readBytes = read(0, &buf, sizeof(buf)) > 0)) {
			if(numbers && toPrint) {
				printf("%d ", i);
				i++;
				toPrint = false;
			}

			if(write(1, &buf, sizeof(buf)) == -1)
				err(1, "ERROR: writing to stdout");

			if(numbers && buf == '\n')
				toPrint = true;
		}
		if(readBytes != 0)
			err(1, "ERROR: reading from stdin");
	} else {
		for(el; el < argc; ++el) {
			int fd;
			if(strcmp("-", argv[el]) == 0)
				fd = 0;
			else 
				if((fd = open(argv[el], O_RDONLY)) == -1)
					err(2, "ERROR: opening file for read: %s", argv[el]);
			
			while((readBytes = read(fd, &buf, sizeof(buf)) > 0)) {
				if(numbers && toPrint) {
					printf("%d ", i);
					i++;
					toPrint = false;
				}

				if(write(1, &buf, sizeof(buf)) == -1)
					err(2, "ERROR: writing to stdout");

				if(numbers && buf == '\n')
					toPrint = true;
			}
			if(readBytes != 0)
				err(2, "ERROR: reading from stdin");

			close(fd);
		}	
	}

	exit(0);
}
