#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>

struct Header {
	uint32_t id;
	uint16_t unused1, unused2;
	uint32_t begin, end;
};

struct Header h;

uint32_t hSize = sizeof(h);
uint32_t maxElements = 16384;

struct Pair {
	int index;
	uint32_t id, len;
};

uint32_t calculateMean(struct Pair* arr, uint32_t size);
uint32_t calculateMean(struct Pair* arr, uint32_t size) {
	uint32_t sum = 0;
	for(uint32_t i = 0; i < size; ++i)
		sum += arr[i].len;

	return sum / size;
}

int calculateD(struct Pair* arr, uint32_t size);
int calculateD(struct Pair* arr, uint32_t size) {
	uint32_t mean = calculateMean(arr, size);
	uint32_t D = 0;
	for(uint32_t i = 0; i < size; ++i) {
		uint32_t x = arr[i].len - mean;
		D += (x*x);
	}

	return D / size;
}
uint32_t getMaxIndex(uint32_t id, struct Pair* arr, uint32_t size);
uint32_t getMaxIndex(uint32_t id, struct Pair* arr, uint32_t size) {
	uint32_t max = 0;
	uint32_t maxIndex = 0;
	for(uint32_t i = 0; i < size; ++i) {
		if(arr[i].index == -1)
			continue;

		if(arr[i].id == id) {
			arr[i].index = -1;
			if(arr[i].len > max) {
				max = arr[i].len;
				maxIndex = i;
			}
		}
	}

	return maxIndex;
}

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count");
	char* file = argv[1];

	struct stat st;	
	if(stat(file, &st) == -1)
		err(2, "ERROR: running stat on file1: %s", file);
	if(st.st_size % hSize != 0)
		errx(3, "ERROR: invalid file1 structure: %s", file);
	if(st.st_size / hSize > maxElements)
		errx(4, "ERROR: elements count must be < 16384");

	struct Pair arr[maxElements];
	for(uint32_t i = 0; i < maxElements; ++i)
		arr[i].index = -1; 

	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(5, "ERROR: opening file 1 for read: %s", file);

	uint32_t index = 0;
	int bytes = 0;
	while((bytes = read(fd, &h, sizeof(h))) > 0) {
		struct Pair p = {index, h.id, h.end - h.begin};
		arr[index++] = p;
	}
	if(bytes < 0)
		err(6, "ERROR: reading from file1: %s", file);

	uint32_t D = calculateD(arr, index);

	for(uint32_t i = 0; i < index; ++i) {
		if(arr[i].index == -1)
			continue;

		uint32_t maxIndex = getMaxIndex(arr[i].id, arr, index);
		
		uint32_t id = arr[maxIndex].id;
		uint32_t len = arr[maxIndex].len;

		if((len * len) > D)
			if(printf("ID: %d; Length: %d", id, len) == -1)
				err(7, "ERROR: writing to stdout");
	}	

	close(fd);

	return 0;
}
