#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

struct Header {
	char name[8];
	uint32_t offset, len;
};

struct Header h;

int openFile(const char* file);
int openFile(const char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(3, "ERROR: running stat on file: %s", file);

	if(st.st_size % sizeof(h) != 0)
		errx(4, "ERROR: size of file not devisible by %ld", sizeof(h));

	if(st.st_size / sizeof(h) > 8)
		errx(5, "ERROR: file triplets are more then 8: %s", file);

	return fd;
}

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count. 1 expected");

	int fd = openFile(argv[1]);

	int pd[2];
	if(pipe(pd) == -1)
		err(8, "ERROR: running pipe() in parent process");

	int bytes = 0;
	while((bytes = read(fd, &h, sizeof(h))) > 0) {
		pid_t pid = fork();

		if(pid < 0)
			err(9, "ERROR: creating a child process");

		if(pid == 0) {
			close(pd[0]);
			close(fd);

			if((fd = open(h.name, O_RDONLY)) == -1)
				err(10, "ERROR: opening file for read in child: %s", h.name);

			if(lseek(fd, h.offset*2, SEEK_SET) == -1)
				err(11, "ERROR: running lseek for file: %s", h.name);
			
			uint16_t buf = 0x0000;
			uint16_t result = 0x0000;

			for(uint32_t i = 0; i < h.len; ++i) {
				bytes = read(fd, &buf, sizeof(buf));
				if(bytes < 0)
					err(12, "ERROR: reading elements of file: %s", h.name);

				result = result ^ buf;
			}

			if(write(pd[1], &result, sizeof(result)) < 0)
				err(13, "ERROR: writing result of file to parent: %s", h.name);

			close(pd[1]);
			exit(0);
		}
	}
	if(bytes < 0)
		err(6, "ERROR: reading triplets from file: %s", argv[1]);

	close(pd[1]);

	uint16_t buf = 0x0000;
	uint16_t result = 0x0000;
	while((bytes = read(pd[0], &buf, sizeof(buf))) > 0) {
		result = result ^ buf;
	}

	while(wait(NULL) > 0) {}

	printf("%dB\n", result);

	close(pd[0]);
	close(fd);
	return 0;
}
