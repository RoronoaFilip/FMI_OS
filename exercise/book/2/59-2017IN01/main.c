#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>

struct pair {
	uint16_t offset;
	uint8_t len;
	uint8_t res;
};

int main(int argc, char** argv) {
	if (argc != 5)
		errx(2, "ERROR: params count");

	int fd1;
	if((fd1 = open(argv[1], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[1]);

	int fd2;
	if((fd2 = open(argv[2], O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", argv[2]);

	int fd3;
	if((fd3 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR, S_IWUSR)) == -1)
		err(2, "ERROR: opening file for write: %s", argv[3]);
	
	int fd4;
	if((fd4 = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR, S_IWUSR)) == -1)
		err(2, "ERROR: opening file for write: %s", argv[4]);

	exit(0);
}
