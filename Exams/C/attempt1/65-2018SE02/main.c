#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>

void mySwap(uint32_t *a, uint32_t *b) {
	uint32_t temp = *a;
	*a = *b;
	*b = temp;
}

void selSort(uint32_t* arr, int size) {
	for(int i = 0; i < size; ++i) {
		int minIndex = i;
		for(int j = i + 1; j < size; ++j) {
			if(arr[j] < arr[minIndex])
				minIndex = j;
		}

		if(minIndex != i)
			mySwap(&arr[i], &arr[minIndex]);
	}
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count");

	struct stat s;
	if(stat(argv[1], &s) == -1)
		errx(1, "ERROR: running stat on file 1: %s", argv[1]);

	if(s.st_size / 4 > 4194304)
		errx(1, "ERROR: file 1 is too big: %s", argv[1]);
	
	int arrSize = s.st_size / 4;

	int fd1;
	if((fd1 = open(argv[1], O_RDONLY)) == -1)
		err(2, "ERROR: opening file 1 for read: %s", argv[1]);

	uint32_t* arr = (uint32_t*) malloc(arrSize);
	int bytes;
	for(int i = 0; i < arrSize; ++i) {
		if((bytes = read(fd1, &arr[i], sizeof(arr[i]))) == -1)
			err(2, "ERROR: reading from file 1: %s", argv[1]);
	}

	selSort(arr, arrSize);

	int fd2;
	if((fd2 = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR + S_IWUSR)) == -1)
		 err(3, "ERROR: opening file 2 for write: %s", argv[2]);

	 for(int i = 0; i < arrSize; ++i) 
	 	 if((bytes = write(fd2, &arr[i], sizeof(arr[i]))) == -1)
	 	 	 err(2, "ERROR: writing to file 2: %s", argv[2]);

	close(fd1);
	close(fd2);

	return 0;
}
