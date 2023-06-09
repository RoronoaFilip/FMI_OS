#include <fcntl.h>
#include <err.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

struct header {
	char name[8];
	uint32_t offset;
	uint32_t length;
};

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count");

	struct stat st;

	if(stat(argv[1], &st) < 0)
		errx(1, "ERROR: running stat on file: %s", argv[1]);

	struct header h;
	if(st.st_size % sizeof(h) != 0 || st.st_size > 8)
		errx(1, "ERROR: invalid input file. More then 8 triplets or size not divisible by 16");

	int fd;
	if((fd = open(argv[1], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[1]);

	int pd[2];
	if(pipe(pd) == -1)
		err(4, "ERROR: running pipe()");

	int result = 0, bytes = 0;
	while((bytes = read(fd, &h, sizeof(h))) > 0) {
		pid_t pid = fork();
		if(pid == 0) {
			close(pd[0]);

			int fd1;
			if((fd1 = open(h.name, O_RDONLY)) == -1)
				err(5, "ERROR: opening file for read: %s", h.name);

			if(lseek(fd1, h.offset * 2, SEEK_SET) == -1)
				err(6, "ERROR: running lseek() in child");

			uint16_t buf;
			for(uint32_t i = 0; i < h.length; ++i) {
				if(read(fd1, &buf, sizeof(buf)) < 0)
					err(7, "ERROR: raeding from file: %s in child", h.name);

				if(write(pd[1], &buf, sizeof(buf)) < 0)
					err(8, "ERROR: writing to pipe while reading from file: %s", h.name);
			}

			close(fd1);
			close(pd[1]);
			exit(0);
		}
	}
	if(bytes < 0)
		err(3, "ERROR: reading from file: %s", argv[2]);

	close(pd[1]);
	uint16_t buf = 0x0000;
	while((bytes = read(pd[0], &buf, sizeof(buf))) > 0) {
		result = result ^ buf;
	}
	if(bytes < 0)
		err(9, "ERROR: reading from pipe in parent");

	while(wait(NULL)) {}

	printf("%dB\n", result);

	close(pd[0]);
	close(fd);
	return 0;
}
