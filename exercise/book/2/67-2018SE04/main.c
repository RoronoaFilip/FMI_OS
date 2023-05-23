#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>

void mySwap(uint16_t* a, uint16_t* b) {
	uint16_t temp = *a;
	*a = *b;
	*b = temp;
}

void selSort(uint16_t* arr, size_t numsCount) {
	for(size_t i = 0; i < numsCount - 1; ++i) {
		size_t minIndex = i;
		for(size_t j = i + 1; j < numsCount; ++j)
			if(arr[j] < arr[minIndex])
				minIndex = j;
		
		if(minIndex != i)
			mySwap(&arr[i], &arr[minIndex]);
	}
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count");

	struct stat s;
	if(stat(argv[1], &s) == -1)
		errx(2, "ERROR: running stat on file: %s", argv[1]);

	size_t fileSize = s.st_size;
	size_t numsCount = fileSize / 2;

	if(numsCount > 65535)
		errx(3, "ERROR: file: %s has more than 65535 numbers", argv[1]);

	uint16_t *arr = (uint16_t*) malloc(fileSize);

	int fd1;
	if((fd1 = open(argv[1], O_RDONLY)) == -1)
		err(4, "ERROR: opening file for read: %s", argv[1]);

	if(read(fd1, arr, fileSize) == -1)
		err(5, "ERROR: reading from file: %s", argv[1]);
	
	selSort(arr, numsCount);

	int fd2;
	if((fd2 = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR + S_IWUSR)) == -1)
		err(6, "ERROR: opening file for write: %s", argv[2]);
	
	if(write(fd2, arr, fileSize) == -1)
		err(7, "ERROR: writing to file: %s", argv[2]);

	free(arr);
	close(fd1);
	close(fd2);

	return 0;
}
