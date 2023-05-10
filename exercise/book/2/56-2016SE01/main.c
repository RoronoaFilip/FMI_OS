#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char** argv) {
	if (argc != 2) 
		errx(2, "Number of params must be 1");

	u_int32_t arr[256];
	for(int i = 0; i < 256; ++i)
		arr[i] = 0;

	int fd1;
	if((fd1 = open(argv[1], O_RDWR)) == -1)
		err(2, "Error opening file");

	int bytesRead = 0;
	uint8_t c = 0;
	while((bytesRead = read(fd1, &c, sizeof(c))) > 0) {
		arr[c]++;	
	}

	if(bytesRead != 0) 
		errx(2, "Error while reading file: %s", argv[1]);

	int lseekCheck = lseek(fd1, 0, SEEK_SET);
	if(lseekCheck == -1)
		errx(2, "Error returning lseek to begin of file %s", argv[1]);

	for(uint8_t i = 0; i < 256; ++i) {
		uint8_t buf = i;
		while(arr[i]--) {
			if((write(fd1, &buf, sizeof(buf))) == -1)
				err(4, "Error while sorting array");
		}
	}

	close(fd1);
	exit(0);
}
