#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>

void myLseek(int fd, int pos);
void myLseek(int fd, int pos) {
	if(lseek(fd, 4 * pos, SEEK_SET) == -1)
		err(3, "ERROR: running lseek");
}

int myWrite(int fd, uint32_t* buf);
int myRead(int fd, uint32_t* buf);

int myWrite(int fd, uint32_t* buf) {
	int bytes = 0;
	if((bytes = write(fd, buf, sizeof(*buf))) == -1)
		err(4, "ERROR: writing to file");

	return bytes;
}
int myRead(int fd, uint32_t* buf) {
	int bytes = 0;	
	if((bytes = read(fd, buf, sizeof(*buf))) == -1)
		err(5, "ERROR: reading from file");
	
	return bytes;
}

void mySwap(int fd, uint32_t n1, uint32_t n2, int pos1, int pos2);
void mySwap(int fd, uint32_t n1, uint32_t n2, int pos1, int pos2) {
	myLseek(fd, pos1);
	myWrite(fd, &n2);

	myLseek(fd, pos2);
	myWrite(fd, &n1);
}

uint32_t minElement(int fd, int* minIndex, uint32_t* old, int size);
uint32_t minElement(int fd, int* minIndex, uint32_t* old, int size) {
	myLseek(fd, *minIndex);

	myRead(fd, old);

	uint32_t min = *old;
	uint32_t buf;
	for(int i = *minIndex +1; i < size; ++i) {
		int bytes = myRead(fd, &buf);

		if(buf < min) {
			min = buf;
			*minIndex = i;
		}
	}

	return min;
}

int main(int argc, char** argv) {

	if(argc != 2)
		errx(1, "ERROR: params count. 1 expected");

	int fd;
	if((fd = open(argv[1], O_RDWR)) == -1)
		err(2, "ERROR: opening file for read/write");

	struct stat st;
	if(stat(argv[1], &st) == -1)
		err(4, "ERROR: running stat in fille");

	if(st.st_size % 4 != 0)
		err(6, "ERROR: file size not devisible by 4");

	int size = st.st_size / 4;

	uint32_t old;
	for(int i = 0; i < size - 1; ++i) {
		int minIndex = i;

		uint32_t minEl = minElement(fd, &minIndex, &old, size);

		if(minIndex != i)
			mySwap(fd, old, minEl, i, minIndex);
	}

	close(fd);
	return 0;
}
