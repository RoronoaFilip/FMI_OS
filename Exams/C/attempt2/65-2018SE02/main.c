#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>

int fSize = 0;

int openFile(const char* file);
int openFile(const char* file) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(3, "ERROR: running stat on file1: %s", file);

	int size = st.st_size;

	if(size % 4 != 0)
		errx(4, "ERROR: file1 size no devisible by 4");

	if(size / 4 > 4194304)
		errx(5, "ERROR: file1 elements are more then 4194304");

	fSize = size / 4;

	return fd;
}

int createFile(const char* file);
int createFile(const char* file) {
	int fd;
	if((fd = open(file, O_CREAT | O_RDWR | O_TRUNC, 0644)) == -1)
		err(6, "ERROR: creating file2 for write: %s", file);

	return fd;
}

void setSeek(int fd, int pos);
void setSeek(int fd, int pos) {
	if(lseek(fd, 4 * pos, SEEK_SET) == -1)
		err(7, "ERROR: running lseek");
}

int myRead(int fd, uint32_t* buf);
int myRead(int fd, uint32_t* buf) {
	int bytes = 0;
	if(( bytes = read(fd, buf, sizeof(*buf))) < 0)
		err(9, "ERROR: reading numbers from file");

	return bytes;
}

int myWrite(int fd, uint32_t buf);
int myWrite(int fd, uint32_t buf) {
	int bytes = 0;
	if((bytes = write(fd, &buf, sizeof(buf))) < 0)
		err(10, "ERROR: writing to file");

	return bytes;
}

uint32_t findMinElement(int fd, int* index, uint32_t* old);
uint32_t findMinElement(int fd, int* index, uint32_t* old) {
	setSeek(fd, *index);

	uint32_t buf;
	myRead(fd, &buf);
	
	*old = buf;
	uint32_t min = buf;
	int minIndex = *index;

	for(int i = *index + 1; i < fSize; ++i) {
		myRead(fd, &buf);

		if(min > buf) {
			min = buf;;
			*index = i;
		}
	}

	setSeek(fd, minIndex);

	return min;
}

void mySwap(int fd, uint32_t el1, uint32_t el2, int pos1, int pos2);
void mySwap(int fd, uint32_t el1, uint32_t el2, int pos1, int pos2) {
	setSeek(fd, pos1);
	myWrite(fd, el2);

	setSeek(fd, pos2);
	myWrite(fd, el1);
	
	setSeek(fd, pos1);
}

int main(int argc, char** argv) {

	if(argc != 3)
		errx(1, "ERROR: params count. 2 expected");
	
	int fd1 = openFile(argv[1]);
	int fd2 = createFile(argv[2]);

	uint32_t buf;
	int bytes = 0;
	while((bytes = myRead(fd1, &buf)) > 0) {
		myWrite(fd2, buf);
	}
	
	setSeek(fd2, 0);

	for(int i = 0; i < fSize - 1; ++i) {
		int minIndex = i;
		uint32_t old;

		uint32_t minEl = findMinElement(fd2, &minIndex, &old);

		if(minIndex != i)
			mySwap(fd2, old, minEl, i, minIndex);
	}

	close(fd1);
	close(fd2);
	return 0;
}
