#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <err.h>
#include <string.h>
#include <stdio.h>

void readAndWrite(int fd, bool N, int* line);
void readAndWrite(int fd, bool N, int* line) {
	bool toPrint = true;

	char buf;
	int bytes;

	while((bytes = read(fd, &buf, sizeof(buf))) > 0) {
		if(N && toPrint) {
			printf("%d ", *line);
			*line = *line + 1;
			toPrint = false;
		}

		if(write(1, &buf, sizeof(buf)) == -1)
			err(5, "ERROR: writing to stdout");

		if(N && buf == '\n')
			toPrint = true;
	}
	if(bytes < 0)
		err(2, "ERROR: reading characters and writing to stdout");
}

int main(int argc, char** argv) {
	setbuf(stdout, NULL);

	bool N = false;
	bool isIn = false;

	if(argc == 1)
		isIn = true;
	else if(argc == 2 && strcmp("-n", argv[1]) == 0) {
		isIn = true;
		N = true;
	} else if(strcmp("-n", argv[1]) == 0)
		N = true;

	char** params = argv + 1 + N;
	int size = argc - 1 - N;	

	int line = 1;
	if(isIn) {
		readAndWrite(0, N, &line);
	} else {
		for(int i = 0; i < size; ++i) {
			int fd;
			if(strcmp("-", params[i]) == 0)
				fd = 0;
			else
				if((fd = open(params[i], O_RDONLY)) == -1)
					err(10, "ERROR: opening file for read: %s", params[i]);

			readAndWrite(fd, N, &line);

			if(fd != 1)
				close(fd);
		}
	}

	return 0;
}
