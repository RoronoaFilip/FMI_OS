//A Program that does the cat command

#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
	
	if (argc == 1) {
		errx(1, "No Files in input");
	}

	for (int i = 1; i < argc; ++i) {
		int fd = open(argv[i], O_RDONLY);
		char c;
		ssize_t bytesRead;
		while((bytesRead = read(fd, &c, sizeof(c))) > 0) {
			ssize_t returnedValue = write(1, &c, sizeof(c));	
			if (returnedValue == -1) {
				err(2, "Error while writing to file");
			}
		}

		if (bytesRead != 0) {
			err(3, "Error while reading file");
		}

		close(fd);
	}

	exit(0);
}
