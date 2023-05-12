#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>

struct pair {
	uint16_t offset;
	uint8_t first;
	uint8_t second;
};

int main(int argc, char** argv) {

	if(argc != 4)
		errx(1, "ERROR: params count");

	struct stat s1;
	struct stat s2;
	if(stat(argv[1], &s1) == -1)
		errx(1, "ERROR: stat on file 1: %s", argv[1]);
	if(stat(argv[2], &s2) == -1)
		errx(1, "ERROR: stat on file 2: %s", argv[2]);

	if(s1.st_size != s2.st_size)
		errx(1, "ERROR: file 1 and file 2 have different sizes");

	int fd1;
	if((fd1 = open(argv[1], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[1]);

	int fd2; 
    if((fd2 = open(argv[2], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[2]);

	int fd3;
    if((fd3 = open(argv[3], O_CREAT | O_WRONLY, S_IRUSR + S_IWUSR)) == -1)
		err(2, "ERROR: opening file for write: %s", argv[3]);

	struct pair p;
	for(p.offset = 1; p.offset <= s1.st_size; ++p.offset) {
		
		if(read(fd1, &p.first, sizeof(p.first)) == -1)
			err(3, "ERROR: reading from file 1: %s", argv[1]);
		if(read(fd2, &p.second, sizeof(p.second)) == -1)
			err(3, "ERROR: reading from file 2: %s", argv[2]);

		if(p.first != p.second)
			if(write(fd3, &p, sizeof(p)) == -1)
				err(4, "ERROR: writing to file 3: %s", argv[3]);
	}

	exit(0);
}
