#include <err.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

struct pair {
	uint16_t offset;
	uint8_t original;
	uint8_t replace;
};

void freeAll(int fd1, int fd2, void* arr) {
	close(fd1);
	close(fd2);
	free(arr);
}

int main(int argc, char** argv) {
	if(argc != 4)
		errx(1, "ERROR: params count");

	struct stat s;
	if(stat(argv[1], &s) == -1)
		errx(1, "ERROR: running stat ot file: %s", argv[1]);

	struct pair p;
	if(s.st_size % sizeof(p) != 0)
		errx(1, "ERROR: file %s's size not devisible by the size of the struct", argv[1]);

	int fd1;
	if((fd1 = open(argv[1], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[1]);
	
	int fd2;
	if((fd2 = open(argv[2], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[2]);

	if(stat(argv[2], &s) == -1)
		errx(1, "ERROR: running stat ot file: %s", argv[2]);

	uint8_t* arr = (uint8_t*)malloc(s.st_size);
	for(int i = 0; i < s.st_size; ++i) {
		if(read(fd2, &arr[i], sizeof(arr[i])) == -1)
			err(3, "ERROR: reading from file 2: %s", argv[2]);
	}

	int readBytes;
	while((readBytes = read(fd1, &p, sizeof(p))) > 0) {
		if(p.offset >= s.st_size) {
			freeAll(fd1, fd2, arr);
			err(4, "ERROR: invalid offset in file 1: %d", p.offset);
		}

		uint16_t index = p.offset - 1;
		if(arr[index] != p.original) {
			freeAll(fd1, fd2, arr);
			err(4, "ERROR: invalid original byte in file 1: %d", p.original);
		}

		arr[index] = p.replace;	
	}
	if(readBytes != 0)
		err(3, "ERROR: reading from file 1: %s", argv[1]);
	
	int fd3;
	if((fd3 = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR + S_IWUSR)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[3]);
	for(int i = 0; i < s.st_size; ++i) {
		if(write(fd3, &arr[i], sizeof(arr[i])) == -1)
			err(5, "ERROR: writing to file 3: %s", argv[3]);
	}

	freeAll(fd1, fd2, arr);
	close(fd3);
	return 0;
}
