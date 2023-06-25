#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <stdint.h>

int size;

void setSeek(int fd, int index);
void setSeek(int fd, int index) {
	if(lseek(fd, index*4, SEEK_SET) == -1)
		err(6, "ERROR: running lseek");
}

int myRead(int fd, uint32_t* buf);
int myRead(int fd, uint32_t* buf) {
	int bytes;
	if((bytes = read(fd, buf, sizeof(*buf))) == -1)
		err(7, "ERROR: reading from file");

	return bytes;
}

int myWrite(int fd, uint32_t* buf);
int myWrite(int fd, uint32_t* buf) {
	int bytes;
	if((bytes = write(fd, buf, sizeof(*buf))) == -1)
		err(7, "ERROR: writing to file");

	return bytes;
}

uint32_t findMinIndex(int fd, uint32_t* curr, int* minIndex);
uint32_t findMinIndex(int fd, uint32_t* curr, int* minIndex) {
	int currIndex = *minIndex;
	setSeek(fd, *minIndex);
	
	uint32_t buf;
	int bytes = myRead(fd, &buf);
	uint32_t min = buf;
	*curr = buf;
	for(int i = *minIndex + 1; i < size; ++i) {
		bytes = myRead(fd, &buf);

		if(buf < min) {
			min = buf;
			*minIndex = i;
		}
	}

	setSeek(fd, currIndex);

	return min;
}

void mySwap(int fd, uint32_t n1, uint32_t n2, int pos1, int pos2);
void mySwap(int fd, uint32_t n1, uint32_t n2, int pos1, int pos2) {
	setSeek(fd, pos2);
	int bytes = myWrite(fd, &n1);

	setSeek(fd, pos1);
	bytes = myWrite(fd, &n2);

	setSeek(fd, pos1);
}

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count");
	
	char* file = argv[1];

	struct stat st;
	if(stat(file, &st) == -1)
		err(2, "Eroor; running stat on file1: %s", file);
	
	if(st.st_size % 4 != 0)
		errx(3, "ERROR: file size not devisible by 4");

	if(st.st_size / 4 > 100000000)
		errx(4, "ERROR: file is too large");

	int fd;
	if((fd = open(file, O_RDWR)) == -1)
		err(5, "ERROR: opening file1 for read: %s", file);

	size = st.st_size / 4;

	for(int i = 0; i < size - 1; ++i) {
		int minIndex = i;
		uint32_t curr = 0;	
		uint32_t new = findMinIndex(fd, &curr, &minIndex);

		if(minIndex != i)
			mySwap(fd, curr, new, i, minIndex);
	}	

	close(fd);
	return 0;
}
