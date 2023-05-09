#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {

	if(argc != 3) {
		errx(1, "err");
	}

	int fd1 = open(argv[1], O_RDONLY);
	if (fd1 == -1) {
		err(2, "Cannot open file 1");
	}
	
	int fd2 = open(argv[2], O_TRUNC | O_WRONLY);
	if (fd2 == -1) {
		err(2, "Cannot open file 2");
	}
	
	char c;
	ssize_t readBytes;

	while ((readBytes = read(fd1, &c, sizeof(c))) > 0) {
		if ((write(fd2, &c, sizeof(c)) == -1)) {
			err(3, "Error while writing to file 2");
		}
	}
	if (readBytes != 0) {
		err(3, "Error ocurred while reading file 1");
	}

	close(fd1);
	close(fd2);
	exit(0);
}
