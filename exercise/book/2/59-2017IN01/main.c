#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

struct pair {
	uint16_t offset;
	uint8_t len;
	uint8_t res;
};

int main(int argc, char** argv) {
	setbuf(stdout, NULL);

	if (argc != 5)
		errx(2, "ERROR: params count");

	int fd1;
	if((fd1 = open(argv[1], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[1]);

	int fd2;
	if((fd2 = open(argv[2], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[2]);

	int fd3;
	if((fd3 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR + S_IWUSR)) == -1)
		err(2, "ERROR: opening file for write: %s", argv[3]);
	
	int fd4;
	if((fd4 = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR + S_IWUSR)) == -1)
		err(2, "ERROR: opening file for write: %s", argv[4]);

	struct stat s;
	if(fstat(fd2, &s) == -1)
		err(3, "ERROR: getting file size");

	struct pair p;
	if(s.st_size % sizeof(p) != 0)
		err(3, "File size not correct: %s", argv[2]);
	
	int bytesRead=0;
	while((bytesRead = read(fd2, &p, sizeof(p))) > 0) {
		p.offset--;
		if(lseek(fd1, p.offset, SEEK_SET) == -1)
			err(4, "ERROR: lseek in file: %s", argv[1]);
		
		char buf;
		if(read(fd1, &buf, sizeof(buf)) == -1)
			err(5, "ERROR: reading from file: %s", argv[1]);

		if(buf < 'A' || buf > 'Z')
			continue;

		if(write(fd3, &buf, sizeof(buf)) == -1)
			err(6, "ERROR: writing to file: %s", argv[3]);
		for(int i = 1; i < p.len; ++i) {
			if(read(fd1, &buf, sizeof(buf)) == -1)
				err(5, "ERROR: reading from file: %s", argv[1]);
			if(write(fd3, &buf, sizeof(buf)) == -1)
				err(6, "ERROR: writing to file: %s", argv[3]);
		}

		if(write(fd4, &p, sizeof(p)) == -1)
			err(7, "ERROR: writing to file: %s", argv[4]);
	}

	if(bytesRead != 0)
		err(4, "ERROR: reading from file: %s", argv[1]);

	close(fd1);
	close(fd2);
	close(fd3);
	close(fd4);

	exit(0);
}
