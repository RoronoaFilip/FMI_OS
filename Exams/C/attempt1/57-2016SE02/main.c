#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

struct pair {
	uint32_t a,b;
};

int main(int argc, char** argv) {
	if(argc != 4)
		errx(1, "ERROR: params count");

	struct stat s;
	if(stat(argv[1], &s) == -1)
		errx(1, "ERROR: extracting size of file 1: %s", argv[1]);

	if (s.st_size % 8 != 0)
		errx(1, "ERROR: size of file 1: %s, is not devisible by 8", argv[1]);

	int fd1, fd2, fd3;
	if((fd1 = open(argv[1], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[1]);

	if((fd2 = open(argv[2], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[2]);

	if((fd3 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR + S_IWUSR)) == -1)
		err(2, "ERROR: opening file for write|trunc: %s", argv[3]);

	struct pair p;

	uint32_t buf = 0;
	int readBytes = 0;
	while((readBytes = read(fd1, &p, sizeof(p))) > 0) {
		if(lseek(fd2, p.a * sizeof(uint32_t), SEEK_SET) == -1)
			err(3, "ERROR: lseek in file: %s", argv[2]);
		for(uint32_t i = 0; i < p.b; ++ i) {
			if(read(fd2, &buf, sizeof(buf)) == -1)	
				err(4, "ERROR: reading from file: %s", argv[2]);
			if(write(fd3, &buf, sizeof(buf)) == -1)
				err(4, "ERROR: writing from file: %s", argv[3]); 
		}
	}
	
	if(readBytes != 0)
		errx(4, "ERROR: reading from file: %s", argv[1]);

	close(fd1);
	close(fd2);
	close(fd3);
	exit(0);
}
